#pragma once

#include "gta_player.hpp"

namespace Stand
{
	struct CollectablesGiver
	{
		static constexpr int all_types[] = { 0, 1, 2, 3, 4, 5, 6, 8, 9, 10, 16, 17, 19 };

		static int getNumCollectiblesWithType(int type);
		static void giveAllCollectiblesWithType(playerbitset_t targets, int type, bool with_chill_pill = true);
	};
}
