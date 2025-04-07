#pragma once

#include "gta_player.hpp"

namespace Stand
{
#pragma pack(push, 1)
	struct SessionAttributes
	{
		uint32_t user : 14;
		uint32_t visible : 1;
		uint32_t bad_sport : 1;
		uint32_t free_aim : 1;
		compactplayer_t players;
		uint8_t region;
	};
#pragma pack(pop)
}
