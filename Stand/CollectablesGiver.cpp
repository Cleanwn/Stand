#include "CollectablesGiver.hpp"

#include "AbstractPlayer.hpp"
#include "Script.hpp"
#include "script_events.hpp"
#include "tse.hpp"
#include "Util.hpp"

namespace Stand
{
	int CollectablesGiver::getNumCollectiblesWithType(int type)
	{
		switch (type)
		{
		case 2:
			return 2;

		case 4:
			return 21;

		case 5: // Shipwrecks (Outfit Scraps)
			return 7;

		case 9:
			return 100;

		case 16: // Snowmen
			return 25;

		case 17: // G's Cache
			return 1;

		case 19: // Los Santos Tags
			return 5;
		}
		return 10;
	}

	void CollectablesGiver::giveAllCollectiblesWithType(playerbitset_t targets, int type, bool with_chill_pill)
	{
		const auto num = (uint64_t)getNumCollectiblesWithType(type);
		for (uint64_t i = 0; i != num; ++i)
		{
			uint64_t args[] = {
				Util::i32_to_u64_trickery(SE_GIVE_COLLECTIBLE),
				g_player,
				(uint64_t)type,
				i,
				1,
				1,
				1
			};
			TSE(targets, args);

			if (with_chill_pill)
			{
				if (i % 7 == 0)
				{
					Script::current()->yield(200);
				}
			}
		}
	}
}
