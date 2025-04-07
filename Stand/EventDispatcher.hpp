#pragma once

#include "gta_fwddecl.hpp"

struct EventDispatcher
{
	void* opt_a1;
	union
	{
		void(*func)(rage::rlPresenceEvent*);
		void(*func_with_a1)(void* a1, rage::rlPresenceEvent*);
	};
};
static_assert(sizeof(EventDispatcher) == 0x10);
