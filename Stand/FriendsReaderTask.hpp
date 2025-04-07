#pragma once

#include "netTask.hpp"
#include "rlFriend.hpp"

namespace rage
{
	struct FriendsReaderTask : public netTask
	{
		INIT_PAD(netTask, 0x70) uint32_t max_friends;
		PAD(0x70 + 4, 0x80) uint32_t* num_friends;
		/* 0x88 */ uint32_t* total_friends;
		/* 0x90 */ rage::rlFriend* friends;

		void addFriend(const char* name, int64_t rockstar_id, bool is_online, bool is_in_multiplayer)
		{
			if (*num_friends < max_friends)
			{
				strcpy(friends[*num_friends].sc_friend.name, name);
				friends[*num_friends].sc_friend.rockstar_id = rockstar_id;
				friends[*num_friends].sc_friend.relationship = 0x20;
				if (is_online)
				{
					friends[*num_friends].sc_friend.flags |= 0b1;
					if (is_in_multiplayer)
					{
						friends[*num_friends].sc_friend.flags |= 0b10;
						friends[*num_friends].flags |= 0b1;
					}
				}
				++*num_friends;
				++*total_friends;
			}
		}
	};
	static_assert(sizeof(FriendsReaderTask) == 0x90 + 8);
}
