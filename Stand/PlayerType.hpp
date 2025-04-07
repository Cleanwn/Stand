#pragma once

#include "typedecl.hpp"

#include "CommandPlayer.hpp"
#include "rlFriend.hpp"

namespace Stand
{
	struct PlayerType
	{
		enum _ : playertype_t
		{
			SELF,
			FRIEND,
			STRANGER,

			SIZE
		};

		[[nodiscard]] static playertype_t fromRID(int64_t rid)
		{
#if ASSUME_NO_RID_SPOOFING
			/*if (g_player.getRockstarId() == rid)
			{
				return SELF;
			}*/

			for (int i = 0; i != rage::rlFriend::getCount(); ++i)
			{
				if (rage::rlFriend::get(i)->sc_friend.rockstar_id == rid)
				{
					return FRIEND;
				}
			}
#endif

			return STRANGER;
		}
	};
}
