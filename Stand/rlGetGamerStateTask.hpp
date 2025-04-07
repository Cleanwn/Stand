#pragma once

#include "netTask.hpp"

#include "rl.hpp"
#include "struct_base.hpp"

namespace rage
{
	struct rlGetGamerStateTask : public netTask
	{
		INIT_PAD(netTask, 0x32C0) const char* responses[RL_MAX_GAMERS_PER_SESSION];
		PAD(0x32C0 + (32 * 8), 0x33C4) uint32_t num_tasks;
	};
	static_assert(sizeof(rlGetGamerStateTask) == 0x33C4 + 4);
}
