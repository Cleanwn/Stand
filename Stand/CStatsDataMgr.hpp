#pragma once

#include "struct_base.hpp"

// using CStatsDataMgr_StatIterFind_t = bool(__fastcall*)(void* CStatsDataMgr, const hash_t& StatId, StatsIterator& iter);

// StatsIterator iter;
// CStatsDataMgr_StatIterFind(CStatsMgr_sm_StatsData, ATSTRINGHASH("MP0_CHAR_ISACTIVE"), iter);
// drawDebugText(iter.map->elements[iter.cur_idx].data->GetBoolean());

enum StatsType : unsigned int
{
	STAT_TYPE_NONE = 0,
	STAT_TYPE_INT = 1,
	STAT_TYPE_FLOAT = 2,
	STAT_TYPE_STRING = 3,
	STAT_TYPE_BOOLEAN = 4,
	STAT_TYPE_UINT8 = 5,
	STAT_TYPE_UINT16 = 6,
	STAT_TYPE_UINT32 = 7,
	STAT_TYPE_UINT64 = 8,
	STAT_TYPE_TIME = 9,
	STAT_TYPE_CASH = 10,
	STAT_TYPE_PERCENT = 11,
	STAT_TYPE_DEGREES = 12,
	STAT_TYPE_WEIGHT = 13,
	STAT_TYPE_MILES = 14,
	STAT_TYPE_METERS = 15,
	STAT_TYPE_FEET = 16,
	STAT_TYPE_SECONDS = 17,
	STAT_TYPE_CHART = 18,
	STAT_TYPE_VELOCITY = 19,
	STAT_TYPE_DATE = 20,
	STAT_TYPE_POS = 21,
	STAT_TYPE_TEXTLABEL = 22,
	STAT_TYPE_PACKED = 23,
	STAT_TYPE_USERID = 24,
	STAT_TYPE_PROFILE_SETTING = 25,
	STAT_TYPE_INT64 = 26,
};

struct sStatData
{
	virtual ~sStatData() = default;
	virtual void Reset() {}
	virtual void SetInt(const int) {}
	virtual void SetInt64(const int64_t) {}
	virtual void SetFloat(const float) {}
	virtual void SetBoolean(const bool) {}
	virtual void SetUInt8(const uint8_t) {}
	virtual void SetUInt16(const uint16_t) {}
	virtual void SetUInt32(const uint32_t) {}
	virtual void SetUInt64(const uint64_t) {}
	virtual void SetString(const char*) {}
	virtual bool SetUserId(const char*) { return false; }
	virtual int32_t GetInt() const = 0;
	virtual int64_t GetInt64() const = 0;
	virtual float GetFloat() const = 0;
	virtual bool GetBoolean() const = 0;
	virtual int8_t GetUInt8() const = 0;
	virtual uint16_t GetUInt16() const = 0;
	virtual uint32_t GetUInt32() const = 0;
	virtual uint64_t GetUInt64() const = 0;
	virtual const char* GetString() const = 0;
	virtual void _0A8() {}
	virtual void _0B0() {}
	virtual void _0B8() {}
	virtual unsigned int GetType() const = 0;
	virtual const char* GetTypeLabel() const = 0;
	virtual uint32_t GetSizeOfData() const = 0;
};

struct StatsEntry
{
	uint32_t hash;
	PAD(4, 8) sStatData* data;
};
static_assert(sizeof(StatsEntry) == 0x10);

struct StatsMap
{
	PAD(0, 8) StatsEntry* elements;
};

struct StatsIterator
{
	StatsMap* map = nullptr;
	uint32_t cur_idx = 0;

	[[nodiscard]] StatsEntry* operator->() const noexcept
	{
		return &map->elements[cur_idx];
	}
};

class CStatsDataMgr
{
};
