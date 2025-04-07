#pragma once

#include <queue>
#include <unordered_map>

#include "gta_player.hpp"
#include "typedecl.hpp"

namespace Stand
{
	class TickQueues
	{
	public:
		static inline std::unordered_map<compactplayer_t, std::queue<hash_t>> arm_queue = {};
		static inline std::unordered_map<uint64_t, std::pair<bool, time_t>> disarm_queue = {};

		static void onTick();
	};
}
