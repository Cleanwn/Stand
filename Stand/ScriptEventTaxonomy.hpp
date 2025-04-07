#pragma once

#include <unordered_map>

#include "gta_player.hpp"

#include "EventAggregation.hpp"

#define ARGSCOUNT2(x) (x - 2 + 3)
#define ARGSIDX2(x) (x >= 2 ? _args[x - 2 + 3] : _args[x])

#define ARGSCOUNT3(x) x
#define ARGSIDX3(x) _args[x]

namespace Stand
{
	struct ScriptEventTaxonomy
	{
		[[nodiscard]] static bool isDirectPacket(int64_t* args, int32_t args_count);
		[[nodiscard]] static EventAggregation dissect(int64_t* args, int32_t args_count, std::unordered_map<int32_t, std::string>& index_names, player_t sender_id);

		[[nodiscard]] static bool isValidFreemodeMissionId(uint32_t id);
	};
}
