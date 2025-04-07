#include "Auth.hpp"

#include <filesystem>
#include <fstream>

#include <fmt/core.h>

#include <rapidjson/document.h>

#include <soup/CustomEncoding.hpp>
#include <soup/Hwid.hpp>
#include <soup/JsonObject.hpp>
#include <soup/ObfusString.hpp>
#include <soup/sha1.hpp>

#include "Codename.hpp"
#include "CommandDiscord.hpp"
#include "CommandRootName.hpp"
#include "conf.hpp"
#include "FiberPool.hpp"
#include "FileLogger.hpp"
#include "get_appdata_path.hpp"
#include "get_current_time_millis.hpp"
#include "get_seconds_since_unix_epoch.hpp"
#include "Gui.hpp"
#include "HttpRequestBuilder.hpp"
#include "is_session.hpp"
#include "main.hpp"
#include "PointerCache.hpp"
#include "PubSubClient.hpp"
#include "RelayCon.hpp"
#include "reversible_scramble.hpp"
#include "rlGamerInfo.hpp"
#include "RootMgr.hpp"
#include "RootNameMgr.hpp"
#include "Script.hpp"
#include "SessionSpoofing.hpp"
#include "str2int.hpp"
#include "StringUtils.hpp"
#include "Tunables.hpp"
#include "Util.hpp"

#define AUTH_DEBUG false

namespace Stand
{
#define PUB_N_VAL "DD27C9574C0B2CEF20D561972520271F6549F7F0D831127D56E6EC910DC3B0BF341EE81DC4068FE77228CC044312B06364404785D2D2E533ABF7C8A4D24B57AD1DE9FEE3DDD0D078C4ABEDA441487A4231648B691AA597C25F0778D9F81BD4770F3ABA3384253B9DB8214466D2E9E62350CB0451CB981BC5BB4667D82C8649889F65808FD0140D3DA715E298B12085CFAEFE49650F53707AE5E403778B3DF3E8CB91BAC86054410A0A035D214088EC6940F14F1F490C0E20D1EB8B9464C8A598BA691F2A59F7AD047EFA5FB831036A6C8103FD7A1AC75B36B7D1EA645FDA1D7E5F4CEE71D3EF628B3BB480DC7797E45C5688F3B5C6D107E303689F6F269EEDF87CAEE8E0C8B63FB701DAD442A9C7508D10A1D7EB3F05C614DE1CE9D71015B6D4DD0D93EDA67642EB915F4259F5E32205CCD9635D830078D033B97D424C571A4644D6A3CC7F5F9D6662647F5ACA5CAFE8CD2CE2176487A72527B56ED213D4DAF656443828B28D2B6C4406DBD55E2CE097ED6FC4D45D32430E70486DED98C9ED32728E250AF8795A706E4DF3CAEC4E664D8F396CCDEF4B72A4D33A8D36E309F457067DFEB777A915EAAFE4665A6A5EE10C509F966836379EC6BD0DBFC435987D2978BEB61E033F484031A7CCFDC8E5B477C59F4842153D4DA6E366B129BD3BD3570F73965C250C32E939C46C5EEBD22847E8BBDDD720299D1E25FD268197B3EA11"

	static char PUB_N[] = PUB_N_VAL;

	Auth::Auth()
		: public_key(soup::Bigint::fromStringHex(PUB_N, 1024))
	{
		memset(PUB_N, 0, COUNT(PUB_N) - 1);
	}

	bool Auth::isStateConsistent() const
	{
		return g_gui.about_to_update_root_state || isStateConsistentRaw();
	}

	bool Auth::isStateConsistentRaw() const
	{
		if (g_gui.root_state < GUI_FREEONLINE)
		{
			return true;
		}
		if (g_gui.root_state == GUI_FREEONLINE)
		{
			return is_session_or_transition_active() && license_permissions == LICPERM_FREE;
		}
		if (g_gui.root_state == GUI_FREE)
		{
			return !is_session_or_transition_active() && license_permissions == LICPERM_FREE;
		}
		if (g_gui.isRootStateBasicEdition())
		{
			return !activation_key_to_try.empty() && license_permissions == LICPERM_BASIC;
		}
		if (g_gui.isRootStateRegularEdition())
		{
			return !activation_key_to_try.empty() && license_permissions == LICPERM_REGULAR;
		}
		if (g_gui.isRootStateUltimateEdition())
		{
			return !activation_key_to_try.empty() && license_permissions == LICPERM_ULTIMATE;
		}
		return false;
	}

	LicensePermissions Auth::verifyPermSig() const // OBFUS!
	{
		LicensePermissions perms = LICPERM_FREE;
		if (get_seconds_since_unix_epoch() < perm_expiry
			&& perm_sig.size() > 1
			&& !g_gui.killswitched
			)
		{
			std::string perm_level = perm_sig.substr(0, 1);
			std::string sig_hex = perm_sig.substr(1);

			std::string expected_signed{ perm_level };
			expected_signed.push_back(':');
			expected_signed.append(activation_key_to_try);
			expected_signed.push_back(':');
			expected_signed.append(fmt::to_string(perm_expiry));
			expected_signed.push_back(':');
			expected_signed.append(GamerIdentifierV4::fromGamerInfo(*pointers::rlPresence_m_ActingGamerInfo).toString());
			expected_signed.push_back(':');
			expected_signed.append(STAND_VERSION);

			if (rage::atStringHash(public_key.n.toStringHex()) == ATSTRINGHASH(PUB_N_VAL))
			{
				if (public_key.verify<soup::sha1>(expected_signed, soup::Bigint::fromStringHex(sig_hex.data(), sig_hex.size())))
				{
					perms = (LicensePermissions)str2int<LicensePermissions_t>(perm_level, LICPERM_FREE);
					/*if (perms != LICPERM_FREE)
					{
						perms = LICPERM_BASIC;
					}*/
					g_last_successful_sig_check = get_current_time_millis();
				}
			}
		}
		return perms;
	}

	bool Auth::isLatestVersion() const noexcept
	{
		return showed_update_notify.empty();
	}

	static __declspec(noinline) void gotPermSigJob() // OBFUS!
	{
		while (g_gui.isRootUpdatePendingOrInProgress())
		{
			Script::current()->yield();
		}
		g_auth.job_queued = false;
		if (!g_auth.isStateConsistent())
		{
			if (g_gui.root_state == GUI_FREE)
			{
				if (g_gui.unlock_recover_state == 1)
				{
					g_gui.unlock_recover_state = 2;
				}
				g_gui.setApplicableNonFreeRootStateValue();
				g_gui.updateRootName();
				CommandDiscord::instance->update();
				CommandRootName::instance->update();
				g_gui.recursivelyApplyActiveState();
				g_gui.removeTab(g_gui.getActivateTab());
			}
			else
			{
				RootMgr::updateRootState();
			}
		}
	}

	static void onPubSubMessage(std::string&& msg)
	{
		switch (msg.c_str()[0])
		{
#if false
		case 't':
			if (!g_tunables.getBool(ATSTRINGHASH("disable_pubsub_msg.t")))
			{
				Util::toast(msg.substr(1));
			}
			break;
#endif

		case 'c':
			if (!g_tunables.getBool(ATSTRINGHASH("disable_pubsub_msg.c")))
			{
				//g_auth.logOut(true);
				g_auth.next_heartbeat = get_seconds_since_unix_epoch();
			}
			break;

		case 'r':
			if (!g_tunables.getBool(ATSTRINGHASH("disable_pubsub_msg.r"))
				&& !g_relay.isRunning()
				&& g_auth.license_permissions >= LICPERM_REGULAR
				)
			{
				g_relay.setServerAndInit();
			}
			break;
		}
	}

	static __forceinline void processConnectionErrorImpl()
	{
		//g_auth.logOut(true);
		if (!g_auth.showed_connection_error_notify)
		{
			g_auth.showed_connection_error_notify = true;
			Util::toast(Label::combineWithSpace(LOC("AUTH_CE"), LOC("CONHELP")), TOAST_ABOVE_MAP);
		}
	}

	static __declspec(noinline) void processConnectionError() // OBFUS!
	{
		processConnectionErrorImpl();
	}

	bool Auth::processHeartbeatResponse(unixtime_t expiry, std::string&& data) // OBFUS!
	{
#if AUTH_DEBUG
		Util::toast(LIT(data), TOAST_ALL);
#endif
		rapidjson::Document document;
		if (!document.Parse(data.data(), data.size()).HasParseError() && document.IsObject())
		{
			bool provided_fail_reason = false;

			// t - translated message
			// m - literal message (show every time)
			// o - literal message (only show once)
			// r - root name
			// u - unlocks
			// v - update available
			// d - direct packet magic
			// p - remote session peers
			// n - additional sigs to scan for to enable edition-exclusive features
			// a - tunables version
			// s - permission signature

			auto mem_t = document.FindMember("t");
			if (mem_t != document.MemberEnd())
			{
				switch (rage::atStringHash(mem_t->value.GetString()))
				{
				case ATSTRINGHASH("share"):
					Util::toast(LOC("AUTHSHR2"), TOAST_ABOVE_MAP);
					provided_fail_reason = true;
					break;

				case ATSTRINGHASH("VER_UNSUPP"):
					Util::toast(Label::combineWithSpace(LOC("VER_UNSUPP"), LOC("UPDHELP")), TOAST_ABOVE_MAP);
					provided_fail_reason = true;
					break;

				case ATSTRINGHASH("VER_UNSUPP_AUTHONLY"):
					Util::toast(Label::combineWithSpace(LOC("VER_UNSUPP_AUTHONLY"), LOC("UPDHELP")), TOAST_ABOVE_MAP);
					provided_fail_reason = true;
					break;
				}
			}
			if (true)
			{
				auto mem_m = document.FindMember("m");
				if (mem_m != document.MemberEnd())
				{
					Util::toast(LIT(mem_m->value.GetString()), TOAST_ABOVE_MAP);
					provided_fail_reason = true;
				}

				auto mem_o = document.FindMember("o");
				if (mem_o != document.MemberEnd())
				{
					if (!g_auth.once_messages_shown.contains(rage::atStringHash(mem_o->value.GetString())))
					{
						Util::toast(LIT(mem_o->value.GetString()), TOAST_ABOVE_MAP);
						g_auth.once_messages_shown.emplace(rage::atStringHash(mem_o->value.GetString()));
					}
				}

				auto mem_r = document.FindMember("r");
				if (mem_r != document.MemberEnd())
				{
					root_name_container = mem_r->value.GetString();
					RootNameMgr::authed_root_name = root_name_container.c_str();
					if (RootNameMgr::current == RN_NULL)
					{
						RootNameMgr::current = RN_AUTHED;
					}
				}

				auto mem_u = document.FindMember("u");
				if (mem_u != document.MemberEnd())
				{
					unlocks = (uint8_t)mem_u->value.GetInt64();
				}

				auto mem_v = document.FindMember("v");
				if (mem_v != document.MemberEnd())
				{
					std::string updated_version = mem_v->value.GetString();
					if (showed_update_notify != updated_version)
					{
						showed_update_notify = updated_version;
						Util::toast(LANG_FMT("UPDAVAIL", updated_version));
					}
				}

				auto mem_d = document.FindMember("d");
				if (mem_d != document.MemberEnd())
				{
					// Not using GetUint64 because PHP's integer type is signed.
					direct_packet_magic = (uint64_t)mem_d->value.GetInt64();
				}
				// We only receive this for the first heartbeat in a session
				/*else
				{
					direct_packet_magic = 0;
				}*/

				remote_session_peers.clear();
				auto mem_p = document.FindMember("p");
				if (mem_p != document.MemberEnd())
				{
					remote_session_peers.reserve(mem_p->value.GetArray().Size());
					for (const auto& peer : mem_p->value.GetArray())
					{
						if (auto gi = GamerIdentifier::fromString(std::string(peer.GetString(), peer.GetStringLength())))
						{
							remote_session_peers.emplace_back(std::move(gi));
						}
					}
				}

				auto mem_a = document.FindMember("a");
				if (mem_a != document.MemberEnd())
				{
#if TUNABLES_DEBUG
					Util::toast(fmt::format("Auth let us know that tunables version is {}, we have {}.", mem_a->value.GetInt64(), g_tunables.version));
#endif
					if (g_tunables.version != mem_a->value.GetInt64())
					{
						g_tunables.download();
					}
				}

				auto mem_s = document.FindMember("s");
				if (mem_s != document.MemberEnd())
				{
					perm_sig = mem_s->value.GetString();
					perm_expiry = expiry;
					auto perms = verifyPermSig();
					if (perms == LICPERM_FREE)
					{
						license_permissions = LICPERM_FREE;
						Util::toast(LOC("GENFAIL"), TOAST_ABOVE_MAP);
						logOut(false);
					}
					else
					{
						license_permissions = perms;
						if (activation_notify)
						{
							activation_notify = false;

							Util::toast(LOC("ACTVTE_SUCC2"), TOAST_ABOVE_MAP);

							std::ofstream activation_key_out(get_appdata_path(soup::ObfusString("Activation Key.txt")));
							activation_key_out << soup::ObfusString("Stand-Activate-").str();
							activation_key_out << activation_key_to_try;
							activation_key_out.close();
						}
						else if (showed_connection_error_notify)
						{
							Util::toast(LOC("TELEKOM_OFF"), TOAST_ABOVE_MAP);
						}
						activation_key_was_valid = true;
					}
					if (auto e = PointerCache::cache.find(Codename("AZ").toString()); e != PointerCache::cache.end())
					{
						uintptr_t data = e->second ^ PointerCache::xor_magic;
						//PointerCache::cache.erase(e);
						g_auth.reportEvent(Codename("AZ"), fmt::to_string(data));
					}
					if (g_gui.unlock_recover_state == 0)
					{
						g_gui.unlock_recover_state = 1;
					}
					g_gui.initial_auth_done.fulfil();
					if (!job_queued)
					{
						job_queued = true;
						FiberPool::queueJob(&gotPermSigJob);
					}
					PubSubClient::subscribe(activation_key_to_try, &onPubSubMessage);

					return true;
				}
			}
			if (!provided_fail_reason)
			{
				notifyInvalidActivationKey();
			}
			g_gui.initial_auth_done.fulfil();
			logOut(false);
		}
		else
		{
			g_gui.initial_auth_done.fulfil();
			if (license_permissions == LICPERM_FREE)
			{
				logOut(false);
			}
			else
			{
				processConnectionErrorImpl();
			}
		}
		return false;
	}

	void Auth::notifyInvalidActivationKey() const
	{
		if (activation_key_was_valid)
		{
			Util::toast(LOC("ACTVTE_RGN"), TOAST_ABOVE_MAP);
		}
		else
		{
			Util::toast(LOC("ACTVTE_ERR_I2"), TOAST_ABOVE_MAP);
		}
	}

	bool Auth::discoverActivationKey() // OBFUS!
	{
		if (g_auth.activation_key_to_try.empty())
		{
			std::ifstream activation_key_in(get_appdata_path(soup::ObfusString("Activation Key.txt")));
			std::string activation_key{};
			std::getline(activation_key_in, activation_key);
			StringUtils::simplify(activation_key);
			if (activation_key.substr(0, 5) == "stand")
			{
				activation_key.erase(0, 5);
			}
			if (activation_key.substr(0, 8) == "activate")
			{
				activation_key.erase(0, 8);
			}
			if (activation_key.length() == ACTIVATION_KEY_CHARS)
			{
				activation_key_to_try = std::move(activation_key);
				return true;
			}
		}
		return false;
	}

	void Auth::enableNotifications() noexcept
	{
		activation_notify = true;
		showed_connection_error_notify = false;
	}

	void Auth::tryActivationKey()
	{
		sendHeartbeat(SessionSpoofing::getRealSessionId());
	}

	static void processHeartbeatRequestFailure(std::string&& reason)
	{
		std::string msg = soup::ObfusString("Failed to send a request to stand.sh: ").str();
		msg.append(reason);
		g_logger.log(std::move(msg));

		processConnectionError();
		g_auth.sent_next_heartbeat = false;
		g_gui.initial_auth_done.fulfil();
	}

	// Keep perm_valid_for in sync with cron_every_10m.php
	static constexpr unixtime_t perm_valid_for = (31 * 60);
	static constexpr unixtime_t heartbeat_every = (28 * 60);

	using HwidEncoding = soup::CustomEncoding<"S15WUZJYIG4K069EDMVTLOQ7PHBFX8RNC3A2">;

	void Auth::sendHeartbeat(uint64_t session) // OBFUS!
	{
		if (g_tunables.version == 0)
		{
			processHeartbeatRequestFailure("No Shirt No Shoes No Service");
			return;
		}

		HttpRequestBuilder b(HttpRequestBuilder::POST, soup::ObfusString("stand.sh"), soup::ObfusString("/api/heartbeat"));
		//if (pointers::rlPresence_m_ActingGamerInfo->getId() != -1)
		{
			unixtime_t expiry = get_seconds_since_unix_epoch() + perm_valid_for;

			soup::JsonObject obj{};
			obj.add("v", STAND_VERSION);
			obj.add("a", activation_key_to_try);
			obj.add("x", expiry);
			obj.add("i", GamerIdentifierV4::fromGamerInfo(*pointers::rlPresence_m_ActingGamerInfo).toString());
			obj.add("h", std::string(1, 'W') + HwidEncoding::encode(soup::Hwid::get().toBinaryString()));
			obj.add("l", Lang::idToApiCode(Lang::active_id)); // Since 114.6
			if (license_permissions != LICPERM_FREE)
			{
				obj.add("p", license_permissions);
			}
			if (isDifferentSession(session))
			{
				if (session == 0)
				{
					obj.add("s", std::string{});
					direct_packet_magic = 0;
				}
				else
				{
					obj.add("s", Util::to_padded_hex_string(session));
				}
			}
			std::string obj_data = obj.encode();
#if AUTH_DEBUG
			Util::toast(LIT(obj_data), TOAST_ALL);
#endif
			b.setPayload(std::move(obj_data));
			b.setResponseCallback([expiry, session](soup::HttpResponse&& resp)
			{
				if (g_auth.processHeartbeatResponse(expiry, std::move(resp.body)))
				{
					static_assert(heartbeat_every < perm_valid_for);
					g_auth.next_heartbeat = (expiry - (perm_valid_for - heartbeat_every));
					g_auth.remote_session = session;
				}
				g_auth.showed_connection_error_notify = false;
				g_auth.sent_next_heartbeat = false;
			});
			b.setFailCallback(&processHeartbeatRequestFailure);
			b.dispatch();
			g_auth.sent_next_heartbeat = true;
		}
	}

	bool Auth::isDifferentSession(uint64_t session) const
	{
		return session != remote_session
			&& (session != 0
				|| !is_session_transition_active(true)
				)
			;
	}

	bool Auth::isSuiEnabledThisSession() const noexcept
	{
		return stand_user_identification || stand_user_identification_this_session;
	}

	void Auth::reportEvent(const Codename& type, const std::string& data) // OBFUS!
	{
		if (hasApiCredentials())
		{
			soup::JsonObject obj{};
			obj.add("a", activation_key_to_try);
			obj.add("t", type.toString());
			obj.add("d", data);
			HttpRequestBuilder b(HttpRequestBuilder::POST, soup::ObfusString("stand.sh"), soup::ObfusString("/api/event"));
			b.setPayload(obj.encode());
			b.setResponseCallback(&processEventResponse);
			b.setFailCallback([](std::string&&)
			{
				processConnectionError();
			});
			b.dispatch();
		}
	}

	void Auth::processEventResponse(soup::HttpResponse&& resp) // OBFUS!
	{
		if (resp.body == "0")
		{
			g_auth.logOut(true);
			//g_auth.notifyInvalidActivationKey();
		}
	}

	void Auth::logOutManual(Click& click) // OBFUS!
	{
		g_auth.logOut(false);
	}

	static __declspec(noinline) void logOut_job() // OBFUS!
	{
		if (g_gui.unlock_recover_state == 0)
		{
			g_gui.unlock_recover_state = 1;
		}
		if (!g_auth.isStateConsistent())
		{
			RootMgr::updateRootState();
		}
	}

	/* __forceinline */ void Auth::logOut(bool retry_activation)
	{
		PubSubClient::unsubscribe(activation_key_to_try);
		if (retry_activation)
		{
			next_heartbeat = get_seconds_since_unix_epoch() + 1;
		}
		else
		{
			activation_key_to_try.clear();
			activation_key_was_valid = false;

			next_heartbeat = 0;
		}
		perm_sig.clear();
		license_permissions = LICPERM_FREE;
		remote_session = (uint64_t)-1;
		remote_session_peers.clear();
		unlocks = 0;
		g_relay.close();
		FiberPool::queueJob(&logOut_job);
	}
}
