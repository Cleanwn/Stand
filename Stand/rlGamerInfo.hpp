#pragma once

#include <string>

#include "fwddecl.hpp"

#include "rlPeerInfo.hpp"

namespace rage
{
	class rlGamerInfo
	{
	public:
		/* 0x00 */ rlPeerInfo peer;
		/* 0xC8 */ rlGamerHandle local_only_handle; // only for local gamer as of 2824
		/* 0xD8 */ int local_index;
		/* 0xDC */ char name[17]; // 2824: 0x84 -> 0xE4 (+0x60)
		/* 0xED */ uint8_t flags;

		void setRid(int64_t rid) noexcept
		{
			local_only_handle.rockstar_id = peer.address.gamer_handle.rockstar_id = rid;
		}

		[[nodiscard]] std::string getNameForUser() const;
		[[nodiscard]] std::string toString() const;

		[[nodiscard]] std::vector<Stand::NuancedEvent> checkDetections(bool is_self = false);

		[[nodiscard]] rlGamerHandle& getHandle() noexcept { return peer.address.gamer_handle; }
		[[nodiscard]] const rlGamerHandle& getHandle() const noexcept { return peer.address.gamer_handle; }

		[[nodiscard]] bool isHost() const;
		[[nodiscard]] bool isUser() const;

		[[nodiscard]] bool isKnownAdvertiser() const;

		/*template <typename T>
		bool ser(T& bb)
		{
			if (bb.serU64(id)
				&& peer.ser(bb)
				&& handle.ser(bb)
				&& bb.serStr(name, 17)
				&& bb.serI32(local_index, 2)
				)
			{
				// re-align
				uint32_t dummy = 0;
				bb.serU32(dummy, 8 - ((T::StrBitLen(name) + 2) % 8));

				return true;
			}
			return false;
		}*/
	};
	//static_assert(sizeof(rlGamerInfo) == 0xEE);
	static_assert(offsetof(rlGamerInfo, local_index) == 0xD8);
	static_assert(offsetof(rlGamerInfo, name) == 0xDC);
	static_assert(offsetof(rlGamerInfo, flags) == 0xED);
}
