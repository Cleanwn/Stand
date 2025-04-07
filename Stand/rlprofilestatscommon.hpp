#pragma once

#include "atSpanArray.hpp"
#include "rlGamerHandle.hpp"

namespace rage
{
	enum rlProfileStatsType
	{
		RL_PROFILESTATS_TYPE_INVALID = -1,
		RL_PROFILESTATS_TYPE_FLOAT = 3,
		RL_PROFILESTATS_TYPE_INT32 = 4,
		RL_PROFILESTATS_TYPE_INT64 = 0
	};

	class rlProfileStatsValue
	{
	public:
		union ValueUnion
		{
			float m_Float;
			int32_t m_Int32;
			int64_t m_Int64;
		};

		ValueUnion m_Union;
		rlProfileStatsType m_Type;
	};

	class rlProfileStatsRecordBase
	{
	public:
		virtual ~rlProfileStatsRecordBase() = default;

		virtual const rlProfileStatsValue* GetValues() const = 0;
		virtual unsigned GetNumValues() const = 0;

		rlGamerHandle m_GamerHandle;
	};

	template <unsigned T_NUM_COLUMNS>
	class rlProfileStatsFixedRecord : public rlProfileStatsRecordBase
	{
	public:
		virtual ~rlProfileStatsFixedRecord() = default;

		const rlProfileStatsValue* GetValues() const final { return m_Values; }
		unsigned GetNumValues() const final { return T_NUM_COLUMNS; }

		rlProfileStatsValue m_Values[T_NUM_COLUMNS];
	};

	class rlProfileStatsReadResults
	{
	public:
		const int* m_StatIds;
		unsigned m_NumStatIds;
		unsigned m_MaxRows;
		unsigned m_NumRows;
		atSpanArray<rlProfileStatsRecordBase> m_Records;

		rlProfileStatsReadResults()
		{
			Clear();
		}

		void Clear()
		{
			m_StatIds = 0;
			m_NumStatIds = 0;
			m_MaxRows = 0;
			m_NumRows = 0;
		}

		void Reset(const int* statIds, const unsigned numStatIds, const unsigned maxRows, rlProfileStatsRecordBase* records, const unsigned recordSpan)
		{
			m_StatIds = statIds;
			m_NumStatIds = numStatIds;
			m_MaxRows = maxRows;
			m_Records.Init(records, recordSpan, maxRows);
		}
	};
	static_assert(offsetof(rlProfileStatsReadResults, m_NumStatIds) == 8);
}
