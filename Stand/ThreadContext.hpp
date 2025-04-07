#pragma once

#include <cstdint>

namespace Stand
{
	enum ThreadContext : uint8_t
	{
		TC_SCRIPT_NOYIELD,
		TC_SCRIPT_YIELDABLE,
		TC_RENDERER,
		TC_WORKER,
		TC_WNDPROC,
		TC_OTHER,

		TC_APPLYDEFAULTSTATE = TC_SCRIPT_NOYIELD,
	};

	[[nodiscard]] constexpr bool thread_context_is_script(ThreadContext thread_context)
	{
		return thread_context <= TC_SCRIPT_YIELDABLE;
	}

	[[nodiscard]] constexpr bool thread_context_has_game_tls(ThreadContext thread_context)
	{
		return thread_context <= TC_RENDERER;
	}
}
