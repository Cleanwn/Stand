#pragma once

#include "natives_decl.hpp"
#include "XoredNativeHash.hpp"

namespace Stand
{
	// Native hooks that we want to apply both to Stand's and the game's native invoker.
	struct SharedNativeHooks
	{
		using enabler_t = void(*)(XoredNativeHash target, rage::scrNativeHandler detour, rage::scrNativeHandler* pOriginal);
		using disabler_t = void(*)(XoredNativeHash target, rage::scrNativeHandler original);

		inline static bool block_outgoing_typing_indicator_events = false;

		static void enable(enabler_t consumer);
		static void disable(disabler_t consumer);
	};
}
