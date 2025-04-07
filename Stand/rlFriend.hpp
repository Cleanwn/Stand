#pragma once

#include "struct_base.hpp"

namespace rage
{
#pragma pack(push, 1)
	struct rlScPlayer
	{
		/* 0x00 */ char name[51];
		PAD(51, 0x38) int64_t rockstar_id;
		/* 0x40 */ uint32_t relationship; // 0x20 == friend
		/* 0x44 */ uint8_t flags; // 0b1 == IsOnline, 0b10 == IsInSameTitle. NETWORK_IS_FRIEND_IN_SAME_TITLE checks IsInSameTitle && IsInSession.

		[[nodiscard]] bool IsOnline() const noexcept
		{
			return flags & 0b1;
		}
	};
	static_assert(sizeof(rlScPlayer) == 0x44 + 1);

	using rlScFriend = rlScPlayer;

	struct rlFriend
	{
		rlScFriend sc_friend;
		PAD(sizeof(rlScFriend), 0x178) uint32_t flags; // 0b1 == IsInSession / NETWORK_IS_FRIEND_IN_MULTIPLAYER
		PAD(0x17C, 0x180);

		[[nodiscard]] bool IsInSession() const noexcept
		{
			return flags & 0b1;
		}

		[[nodiscard]] static int getCount();
		[[nodiscard]] static rlFriend* get(int idx);
	};
	static_assert(sizeof(rlFriend) == 0x180);
#pragma pack(pop)
}
