#pragma once

#include "typedecl.hpp"

namespace Stand
{
	struct PerTickToggleable
	{
		bool this_tick = false;
		bool every_tick = false;

		[[nodiscard]] bool isActiveThisTick() const noexcept;
		[[nodiscard]] bool onTick() noexcept;
	};

	struct TickMgr
	{
		inline static bool explosion_this_tick = false;

		inline static PerTickToggleable expammo{};
		inline static int expammo_type = 2;

		inline static PerTickToggleable super_jump{};
		inline static bool is_super_jumping = false;
		inline static bool is_super_jumping_for_user = false;

		static void onTick();
		static void onPostTick();

		static void doSuperJump();
	};
}
