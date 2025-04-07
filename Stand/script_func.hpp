#pragma once

// am_mp_rc_vehicle

#define FUNC_RC_ISRCPV			450

// appinternet

#define FUNC_AI_CANPRCHS		7662 // function below: (joaat("terbyte"), -1))

// carmod_shop

#define FUNC_CM_CANENTER		2782
#define FUNC_CM_CANFULLYMOD		2411

// freemode

#define FUNC_FM_ISRCVEH			2023
#define FUNC_FM_DAILY_EXPENSES	11723
#define FUNC_FM_BIGMAP			13641 // mpply_toggle_off_big_radar
#define FUNC_FM_SHOULDHIDEBLIP  15228 // if (iVar0 != iVar1 && !
#define FUNC_FM_SHOULDSHOWBLIP  15974 // bParam0] = NETWORK::GET_TIME_OFFSET(NETWORK::GET_NETWORK_TIME(), -35000);
#define FUNC_FM_AMTT_NSUIT		18987 // "AMTT_NSUIT" followed by `return 0;`
#define FUNC_FM_LDB_ISLAND_CHK  21602 // {DPAD_PASSED_ISLAND_CHECK} if ((bVar0 && !bVar1) || (!bVar0 && bVar1))
#define FUNC_FM_SET_PLY_CNT		21773 // {SET_TOTAL_PLAYER_COUNT_DATA_POST} ((*uParam0 - uParam0->f_3) - iVar0)

#include <cstdint>

namespace Stand
{
	class scrFunc
	{
	public:
		uint32_t logical_start = 0; // including Enter instruction
		uint32_t start = 0; // first instruction after Enter
		uint32_t end = 0; // instruction after final Return

		[[nodiscard]] bool isValid() const noexcept
		{
			return end != 0;
		}

		[[nodiscard]] uint32_t size() const noexcept
		{
			return end - start;
		}

		[[nodiscard]] uint32_t getLogicalSize() const noexcept
		{
			return end - logical_start;
		}
	};
}
