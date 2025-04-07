#pragma once

#include "fwddecl.hpp"

namespace Stand
{
	inline bool g_yeeting_with_abort = false;

	extern void yeet_flow(ThreadContext thread_context);
	extern __declspec(noinline) void yeet_now();
}
