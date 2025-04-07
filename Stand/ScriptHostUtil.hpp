#pragma once

#include <array>

#include "fwddecl.hpp"
#include "typedecl.hpp"
#include "gta_player.hpp"

namespace Stand
{
	class ScriptHostUtil
	{
	public:
		inline static time_t last_script_host_request = 0;

		[[nodiscard]] static bool canSafelyTakeIt();

		static void takeIt();
		[[nodiscard]] static bool takeItYielding();
	private:
		static bool takeItImpl();

	public:
		inline static std::array<time_t, MAX_PLAYERS> pool_closed_at{};

		[[nodiscard]] static AbstractPlayer getViableSh();
	};
}
