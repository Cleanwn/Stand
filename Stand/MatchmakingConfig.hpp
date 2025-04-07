#pragma once

#include "fwddecl.hpp"
#include "gta_player.hpp"

namespace Stand
{
#pragma pack(push, 1)
	struct MatchmakingConfig
	{
		compactplayer_t min_players = 0;
		compactplayer_t max_players = MAX_PLAYERS - NUM_SPECTATOR_SLOTS;
		uint32_t region = -2; // -1 = use rlRos_s_GeoLocInfo, -2 = no region filter
		uint8_t free_aim = 0xFF;
		uint8_t visible = 1;
		uint8_t bad_sport = 0xFF;
		bool filter_user = false;
		uint32_t user;
		bool adblock = true;

		[[nodiscard]] constexpr bool operator==(const MatchmakingConfig& b) const noexcept
		{
			return min_players == b.min_players
				&& max_players == b.max_players
				&& region == b.region
				&& free_aim == b.free_aim
				&& visible == b.visible
				&& bad_sport == b.bad_sport
				&& filter_user == b.filter_user
				&& user == b.user
				&& adblock == b.adblock
				;
		}

		[[nodiscard]] constexpr bool operator!=(const MatchmakingConfig& b) const noexcept
		{
			return !operator==(b);
		}
	};
#pragma pack(pop)
}
