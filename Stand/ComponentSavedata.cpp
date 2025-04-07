#include "ComponentSavedata.hpp"

#ifdef STAND_DEBUG
#include <soup/JsonBool.hpp>
#include <soup/JsonFloat.hpp>
#include <soup/JsonInt.hpp>
#include <soup/UniquePtr.hpp>
#endif

#include "atStringHash.hpp"
#include "CStatsDataMgr.hpp"

#define PRINT_UNSUPPORTED false

#if PRINT_UNSUPPORTED
#include <fmt/format.h>
#include "ThreadContext.hpp"
#include "Util.hpp"
#endif

#include "ComponentImpl.hpp"

namespace Stand
{
	static CStatsDataMgr* stats_data;
	static DetourHook CStatsDataMgr_StatIterFind_hook{ "G2" };
	static DetourHook CStatsSavesMgr_CheckForCloudDirtyRead_hook{ "G3" };

#ifdef STAND_DEBUG
	struct StatDataShim : public sStatData
	{
		int32_t GetInt() const final { return static_cast<int32_t>(data); }
		int64_t GetInt64() const final { return data; }
		float GetFloat() const final { return fdata; }
		bool GetBoolean() const final { return data != 0; }
		int8_t GetUInt8() const final { return static_cast<int8_t>(data); }
		uint16_t GetUInt16() const final { return static_cast<uint16_t>(data); }
		uint32_t GetUInt32() const final { return static_cast<uint32_t>(data); }
		uint64_t GetUInt64() const final { return data; }
		const char* GetString() const final { return nullptr; }
		unsigned int GetType() const final { return type;  }
		const char* GetTypeLabel() const final { return nullptr; }
		uint32_t GetSizeOfData() const final { return 8; }

		StatsType type;
		union
		{
			uint64_t data;
			float fdata;
		};
	};

	static StatDataShim data_shim;

	struct StatsEntryShim : public StatsEntry
	{
		StatsEntryShim()
			: StatsEntry()
		{
			data = &data_shim;
		}
	};

	static StatsEntryShim entry_shim;

	struct StatsMapShim : public StatsMap
	{
		StatsMapShim()
			: StatsMap()
		{
			elements = &entry_shim;
		}
	};

	static StatsMapShim map_shim;
#endif

	static bool __fastcall CStatsDataMgr_StatIterFind(void* CStatsDataMgr, const hash_t& StatId, StatsIterator& iter)
	{
#ifdef STAND_DEBUG
		if (g_comp_savedata.replay)
		{
			if (auto e = g_comp_savedata.obj.find(soup::JsonInt(StatId)); e != nullptr)
			{
				switch (e->type)
				{
				default:
					data_shim.type = STAT_TYPE_UINT64;
					data_shim.data = e->asInt().value;
					break;

				case soup::JSON_BOOL:
					data_shim.type = STAT_TYPE_BOOLEAN;
					data_shim.data = e->asBool().value;
					break;

				case soup::JSON_FLOAT:
					data_shim.type = STAT_TYPE_FLOAT;
					data_shim.fdata = static_cast<float>(e->asFloat().value);
					break;
				}
				entry_shim.hash = StatId;
				iter.map = &map_shim;
				iter.cur_idx = 0;
				return true;
			}
		}
#endif
		if (COMP_OG(CStatsDataMgr_StatIterFind)(CStatsDataMgr, StatId, iter))
		{
			if (StatId == ATSTRINGHASH("mpply_last_mp_char"))
			{
				if (iter->data->GetInt() >= 5)
				{
					//Util::toast("Fixed mpply_last_mp_char");
					iter->data->SetInt(0);
				}
			}
#ifdef STAND_DEBUG
			if (g_comp_savedata.record)
			{
				const auto type = iter->data->GetType();
				soup::UniquePtr<soup::JsonNode> jsonval;
				if (type == STAT_TYPE_BOOLEAN)
				{
					jsonval = soup::make_unique<soup::JsonBool>(iter->data->GetBoolean());
				}
				else if (type == STAT_TYPE_FLOAT)
				{
					jsonval = soup::make_unique<soup::JsonFloat>(iter->data->GetFloat());
				}
				else if (type == STAT_TYPE_INT || type == STAT_TYPE_UINT64)
				{
					jsonval = soup::make_unique<soup::JsonInt>(iter->data->GetUInt64());
				}
#if PRINT_UNSUPPORTED
				else
				{
					Util::toast(fmt::format("Unsupported stats type: {}", type), TC_OTHER);
				}
#endif
				if (jsonval)
				{
					auto jsonkey = soup::make_unique<soup::JsonInt>(StatId);
					if (auto e = g_comp_savedata.obj.findUp(*jsonkey); e != nullptr)
					{
						*e = std::move(jsonval);
					}
					else
					{
						g_comp_savedata.obj.add(std::move(jsonkey), std::move(jsonval));
					}
				}
			}
#endif
			return true;
		}
		return false;
	}

	static bool __fastcall CStatsSavesMgr_CheckForCloudDirtyRead(__int64 a1, int file)
	{
		if (COMP_OG(CStatsSavesMgr_CheckForCloudDirtyRead)(a1, file))
		{
			if (!g_comp_savedata.ignore_dirty_read)
			{
				return true;
			}
		}
		return false;
	}

	void ComponentSavedata::addPatterns(PatternBatch& batch)
	{
		BATCH_ADD(CODENAME(CStatsDataMgr_StatIterFind), "48 8D 0D ? ? ? ? 44 89 6D F8 E8", [](soup::Pointer p)
		{
			stats_data = p.add(3).rip().as<CStatsDataMgr*>();

			p = p.add(12).rip();
			CHECK_EXISTING_HOOK(CODENAME(CStatsDataMgr_StatIterFind), "48 89 5C 24 08");
			INIT_HOOK_OPTIONAL(CStatsDataMgr_StatIterFind);
		});
		BATCH_ADD_OPTIONAL_HOOK(CStatsSavesMgr_CheckForCloudDirtyRead, "48 89 5C 24 08", "48 89 6C 24 10 48 89 74 24 20 57 41 56 41 57 48 83 EC 40 48 8B 05");
	}

	std::vector<DetourHook*> ComponentSavedata::getHooks()
	{
		return {
			&CStatsDataMgr_StatIterFind_hook,
			&CStatsSavesMgr_CheckForCloudDirtyRead_hook,
		};
	}

	sStatData* ComponentSavedata::findStat(hash_t hash) const noexcept
	{
		StatsIterator iter{};
		if (COMP_OG(CStatsDataMgr_StatIterFind)(stats_data, hash, iter))
		{
			return iter->data;
		}
		return nullptr;
	}
}
