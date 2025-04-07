#pragma once

#include "netStatus.hpp"
#include "rlprofilestatscommon.hpp"

#include <soup/UniqueListLink.hpp>

namespace Stand
{
	class RemoteStats
	{
	public:
		const int64_t rid;
		soup::UniqueListLink<RemoteStats> unique_list_link;

	private:
		enum
		{
			LAST_CHAR,
			BANK,
			KILLS,
			DEATHS_1,
			DEATHS_2,
			KD,
			MP0_ISACTIVE,
			MP0_WALLET,
			MP0_RANK,
			MP1_ISACTIVE,
			MP1_WALLET,
			MP1_RANK,
		};

		inline static int statIds[] = {
			ATSTRINGHASH("mpply_last_mp_char"),
			ATSTRINGHASH("BANK_BALANCE"),
			ATSTRINGHASH("mpply_kills_players"),
			ATSTRINGHASH("mpply_deaths_player"),
			ATSTRINGHASH("mpply_deaths_player_suicide"),
			ATSTRINGHASH("mpply_kill_death_ratio"),
			ATSTRINGHASH("MP0_CHAR_ISACTIVE"),
			ATSTRINGHASH("MP0_WALLET_BALANCE"),
			ATSTRINGHASH("MP0_CHAR_RANK_FM"),
			ATSTRINGHASH("MP1_CHAR_ISACTIVE"),
			ATSTRINGHASH("MP1_WALLET_BALANCE"),
			ATSTRINGHASH("MP1_CHAR_RANK_FM"),
		};

		rage::rlProfileStatsReadResults results;
		rage::rlProfileStatsFixedRecord<COUNT(statIds)> record;
		rage::netStatus status;

	private:
		RemoteStats(int64_t rid);

	public:
		[[nodiscard]] static RemoteStats* get(int64_t rid);
		static bool tryFree(RemoteStats*& s);
		[[nodiscard]] static bool shutdown();

		[[nodiscard]] bool isPending() const noexcept
		{
			return status.isPending();
		}

		[[nodiscard]] bool hasData() const noexcept
		{
			return record.m_Values[LAST_CHAR].m_Type != rage::RL_PROFILESTATS_TYPE_INVALID;
		}
		
		[[nodiscard]] const int32_t& getLastChar() const noexcept
		{
			return record.m_Values[LAST_CHAR].m_Union.m_Int32;
		}

		[[nodiscard]] const int64_t& getBankBalance() const noexcept
		{
			return record.m_Values[BANK].m_Union.m_Int64;
		}

		[[nodiscard]] const int32_t& getKills() const noexcept
		{
			return record.m_Values[KILLS].m_Union.m_Int32;
		}

		[[nodiscard]] const int32_t getDeaths() const noexcept
		{
			return record.m_Values[DEATHS_1].m_Union.m_Int32 + record.m_Values[DEATHS_2].m_Union.m_Int32;
		}

		[[nodiscard]] const float& getKillDeathRatio() const noexcept
		{
			return record.m_Values[KD].m_Union.m_Float;
		}

		[[nodiscard]] bool getMp0IsActive() const noexcept
		{
			return record.m_Values[MP0_ISACTIVE].m_Union.m_Int32;
		}

		[[nodiscard]] const int64_t& getMp0WalletBalance() const noexcept
		{
			return record.m_Values[MP0_WALLET].m_Union.m_Int64;
		}

		[[nodiscard]] const int32_t& getMp0Rank() const noexcept
		{
			return record.m_Values[MP0_RANK].m_Union.m_Int32;
		}

		[[nodiscard]] bool getMp1IsActive() const noexcept
		{
			return record.m_Values[MP1_ISACTIVE].m_Union.m_Int32;
		}

		[[nodiscard]] const int64_t& getMp1WalletBalance() const noexcept
		{
			return record.m_Values[MP1_WALLET].m_Union.m_Int64;
		}

		[[nodiscard]] const int32_t& getMp1Rank() const noexcept
		{
			return record.m_Values[MP1_RANK].m_Union.m_Int32;
		}
	};
}
