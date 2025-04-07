#pragma once

#include "netMessage.hpp"

#include "netPeerId.hpp"

#pragma pack(push, 1)
namespace rage
{
	struct netComplaintMsg : public netMessage
	{
		/* 0x000 */ netPeerId sender;
		/* 0x008 */ netPeerId targets[64];
		/* 0x208 */ uint32_t num_targets;
	};
	static_assert(sizeof(netComplaintMsg) == 0x208 + 4);
}
#pragma pack(pop)
