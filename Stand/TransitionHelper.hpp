#pragma once

#include <vector>

#include "fwddecl.hpp"
#include "typedecl.hpp"

#include "SessionType.hpp"

namespace Stand
{
	struct TransitionHelper
	{
		inline static bool fix_stucks = true;
		inline static time_t ts_started = 0;

		[[nodiscard]] static time_t getTimeWastedOnTransitionState();
		[[nodiscard]] static time_t getTimeWhenTransitionEnded();
		[[nodiscard]] static time_t getTimeSinceTransitionEnded();
		static void onTick();

		[[nodiscard]] static bool isPrologueDone();
		static void skipPrologue();

		static bool doTransition(SessionType session_type, bool end_when_almost_done = false);
		static void abortTransition();
		static void forceQuit();

		static void beAlone(int remove_reason = -1);
		static void beAlone(const std::vector<AbstractPlayer>& other_players, int remove_reason = -1);
	};
}
