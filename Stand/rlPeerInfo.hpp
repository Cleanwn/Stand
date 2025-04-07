#pragma once

#include "netPeerAddress.hpp"

namespace rage
{
	struct rlPeerInfo
	{
		/* 0x00 */ netPeerAddress address;
		/* 0xC0 */ netPeerId id; // 0x28 in 323, actually a uint64_t
		                         // smallest value determines next host, see CNetworkSession::GetMigrationCandidates - 41 B8 20 00 00 00 48 8B CB E8 ? ? ? ? 8B E8

		/*template <typename T>
		bool ser(T& bb)
		{
			if (!address.ser(bb))
			{
				return false;
			}

			uint32_t hi = id >> 32;
			auto lo = (uint32_t)id;
			if (!bb.serU32(hi)
				|| !bb.serU32(lo)
				)
			{
				return false;
			}
			id = ((uint64_t)hi << 32) | lo;

			return true;
		}*/
	};
	static_assert(sizeof(rlPeerInfo) == 0xC8);
}
