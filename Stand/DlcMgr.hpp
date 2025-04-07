#pragma once

#include <cstdint>

namespace Stand
{
	struct DlcMgr
	{
		inline static bool dev_flag = false;

		inline static uint8_t preorder_bonus_override = 0;
		inline static bool admin_dlc = false;

		inline static bool has_hook = false;
		[[nodiscard]] static bool isHookNeeded();
		static void update();
	};
}
