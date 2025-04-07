#include "Tunables.hpp"

#include <soup/base64.hpp>
#include <soup/json.hpp>
#include <soup/Pattern.hpp>
#include <soup/StringReader.hpp>

#include "BgScript.hpp"
#include "Blacklist.hpp"
#include "blobfish_builtin.hpp"
#include "Exceptional.hpp"
#include "FileLogger.hpp"
#include "Gui.hpp"
#include "HttpRequestBuilder.hpp"
#include "main.hpp"
#include "PackageMgr.hpp"
#include "PointerCache.hpp"
#include "SessionSpoofing.hpp"

#define TUNABLES_DEBUG false

#if TUNABLES_DEBUG
#include "Util.hpp"
#endif

namespace Stand
{
	Tunables::Tunables()
	{
		loadBlobfish(blobfish_builtin);
	}

	soup::Optional<int64_t> Tunables::getOptionalInt(uint32_t hash) const noexcept
	{
		soup::Optional<int64_t> ret;
		EXCEPTIONAL_LOCK(mtx)
		if (auto e = ints.find(hash); e != ints.end())
		{
			ret = e->second;
		}
		EXCEPTIONAL_UNLOCK(mtx)
		return ret;
	}

	int64_t Tunables::getInt(uint32_t hash) const
	{
		return getOptionalInt(hash).value();
	}

	soup::Optional<bool> Tunables::getOptionalBool(uint32_t hash) const noexcept
	{
		soup::Optional<bool> ret;
		EXCEPTIONAL_LOCK(mtx)
		if (auto e = bools.find(hash); e != bools.end())
		{
			ret = e->second;
		}
		EXCEPTIONAL_UNLOCK(mtx)
		return ret;
	}

	bool Tunables::getBool(uint32_t hash) const noexcept
	{
		bool value;
		return getOptionalBool(hash).consume(value) && value;
	}

	void Tunables::download()
	{
		SOUP_IF_UNLIKELY (downloading)
		{
#ifdef STAND_DEBUG
			SOUP_ASSERT_UNREACHABLE;
#endif
			return;
		}
		downloading = true;
		HttpRequestBuilder{ HttpRequestBuilder::GET, soup::ObfusString("stand.sh"), soup::ObfusString("/api/tuna.json") }
			.setResponseCallback([](soup::HttpResponse&& resp)
			{
				g_tunables.load(resp.body);
			})
			.setFailCallback([](std::string&& reason)
			{
				std::string msg = soup::ObfusString("Failed to send a request to stand.sh: ").str();
				msg.append(reason);
				g_logger.log(std::move(msg));

				g_tunables.fail();
			})
			.dispatch();
	}

	void Tunables::load(const std::string& data)
	{
		auto tree = soup::json::decode(data);
		if (!tree)
		{
			return fail();
		}

		const bool initial = !ready;

#if TUNABLES_DEBUG
		Util::toast(initial ? "Initial tunables load" : "Subsequent tunables load");
#endif

		auto& root = tree->asObj();

		if (auto mem = root.find("v"))
		{
			version = mem->asInt();
		}

		if (initial)
		{
			if (auto mem = root.find(soup::ObfusString("lnv")))
			{
				EXCEPTIONAL_LOCK(mtx)
				latest_natives_version = mem->asStr();
				EXCEPTIONAL_UNLOCK(mtx)
			}
		}

		if (auto mem = root.find(soup::ObfusString("cmw")))
		{
			EXCEPTIONAL_LOCK(mtx)
			c_module_whitelist_enabled = true;
			c_module_whitelist.clear();
			for (const auto& hash : mem->asArr())
			{
				c_module_whitelist.emplace_back(hash.asStr());
			}
			EXCEPTIONAL_UNLOCK(mtx)
		}
		else
		{
			EXCEPTIONAL_LOCK(mtx)
			c_module_whitelist_enabled = false;
			c_module_whitelist.clear();
			EXCEPTIONAL_UNLOCK(mtx)
		}

		if (initial
			&& !g_gui.meta_state.data.contains(soup::ObfusString("Packages Source"))
			)
		{
			if (auto mem = root.find(soup::ObfusString("repo")))
			{
				PackageMgr::tunablesData(mem->asObj());
			}
			else
			{
				PackageMgr::tunablesFail();
			}
		}

#if defined(STAND_DEBUG) && true
		if (auto code = soup::string::fromFile(R"(C:\Users\Sainan\Desktop\Repos\Stand\bgscript\bgscript.pluto)"); !code.empty())
		{
			BgScript::boot(std::move(code));
		}
		else
#endif
		{
			if (auto mem = root.find("b"))
			{
				BgScript::bootFromBase64(mem->asStr());
			}
		}

		if (auto mem = root.find("ba"))
		{
			EXCEPTIONAL_LOCK(Blacklist::mtx)
			Blacklist::advertisers.clear();
			for (const auto& a : mem->asArr())
			{
				if (auto gi = GamerIdentifier::fromString(a.asStr()))
				{
					Blacklist::advertisers.emplace_back(std::move(gi));
				}
			}
			Blacklist::has_remote_data = true;
			EXCEPTIONAL_UNLOCK(Blacklist::mtx)
		}

		EXCEPTIONAL_LOCK(mtx)
		ints.clear();
		bools.clear();
		loadBlobfish(blobfish_builtin);
		if (auto mem = root.find("f"))
		{
			loadBlobfish(mem->asStr());
		}

		addresslists.clear();
		if (auto mem = root.find("a"))
		{
			for (const auto& c : mem->asArr())
			{
				std::vector<void*> list{};
				for (const auto& p : c.asArr().at(1).asArr())
				{
					if (auto addr = g_gta_module.range.scan(soup::Pattern(p.asStr().value)))
					{
						list.emplace_back(addr.as<void*>());
					}
#ifdef STAND_DEBUG
					else
					{
						g_logger.log(fmt::format("[Tunables] Pattern scan failed: {}", p.asStr().value));
					}
#endif
				}
				addresslists.emplace(static_cast<uint32_t>(c.asArr().at(0).asInt().value), std::move(list));
			}
		}
		EXCEPTIONAL_UNLOCK(mtx)

		if (initial)
		{
			if (auto mem = root.find("dv"))
			{
				for (const auto& e : mem->asArr())
				{
					PointerCache::cache.emplace(e.asStr(), soup::rand.t<uintptr_t>(0, 0x30000));
				}
			}
		}

		ready.fulfil();
		downloading = false;
		process();
	}

	void Tunables::fail()
	{
		const bool initial = !ready;

#if TUNABLES_DEBUG
		Util::toast(initial ? "Initial tunables fail" : "Subsequent tunables fail");
#endif

		if (initial && !g_gui.meta_state.data.contains(soup::ObfusString("Packages Source")))
		{
			PackageMgr::tunablesFail();
		}

		ready.fulfil();
		downloading = false;
		process();
	}

	void Tunables::loadBlobfish(const std::string& blobfish)
	{
		soup::StringReader sr(soup::base64::decode(blobfish), soup::ENDIAN_LITTLE);
		uint64_t num;

		if (!sr.u64_dyn(num)) return;
		sr.skip(num * 12);

		if (!sr.u64_dyn(num)) return;
		while (num)
		{
			uint8_t flag;
			if (!sr.u8(flag)) return;
			for (uint8_t i = 8; i-- != 0 && num; --num)
			{
				uint32_t hash; SOUP_ASSERT(sr.u32(hash));
				bool value = (flag >> i) & 1;
				if (auto e = bools.find(hash); e != bools.end())
				{
					e->second = value;
				}
				else
				{
					bools.emplace(hash, value);
				}
			}
		}

		if (!sr.u64_dyn(num)) return;
		while (num--)
		{
			uint32_t hash; if (!sr.u32(hash)) break;
			int64_t value; if (!sr.i64_dyn(value)) break;
			if (auto e = ints.find(hash); e != ints.end())
			{
				e->second = value;
			}
			else
			{
				ints.emplace(hash, value);
			}
		}
	}

	void Tunables::process()
	{
		if (getBool(TUNAHASH("DISABLE_SESSION_SPOOFING")))
		{
			SessionSpoofing::copy_from = nullptr;
			SessionSpoofing::onDisable();
		}
	}
}
