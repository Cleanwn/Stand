#pragma once

#include "rlclancommon.hpp"

#pragma pack(push, 1)
namespace rage
{
	class rlClanClient
	{
		PAD(0x000, 0x020) rage::rlClanMembershipData memberships[8]; // only up to 5 are actually used :'(
		/* 0x5E0 */ int num_memberships;
	};
	static_assert(sizeof(rlClanClient) == 0x5E0 + 4);
}
#pragma pack(pop)
