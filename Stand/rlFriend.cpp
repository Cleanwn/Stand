#include "rlFriend.hpp"

#include "natives.hpp"

namespace rage
{
	int rlFriend::getCount()
	{
		return NETWORK::NETWORK_GET_FRIEND_COUNT();
	}

	rlFriend* rlFriend::get(int idx)
	{
		return reinterpret_cast<rlFriend*>(reinterpret_cast<uintptr_t>(NETWORK::NETWORK_GET_FRIEND_NAME(idx)) - offsetof(rlFriend, sc_friend.name));
	}
}
