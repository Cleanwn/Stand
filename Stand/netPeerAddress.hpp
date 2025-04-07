#pragma once

#include "netPeerId.hpp"
#include "netRelayToken.hpp"
#include "netSocketAddress.hpp"
#include "rlGamerHandle.hpp"

#pragma pack(push, 1)
namespace rage
{
	struct netPeerAddress
	{
		/* 0x00 */ uint8_t unk_2824_1;
		PAD(0x01, 0x08) netPeerId startup_id;
		/* 0x10 */ rlGamerHandle gamer_handle;
		PAD(0x10 + sizeof(rlGamerHandle), 0xA8) netSocketAddress local_only_online; // only on local peer as of 2824
		/* 0xB0 */ netSocketAddress local_only_lan; // only on local peer as of 2824
		/* 0xB8 */ uint32_t nat_type; // 0 for local
		/* 0xBC */ uint8_t unk_2824_2;
		PAD(0xBC + 1, 0xC0);

		void censorOnlineIp() noexcept;

		// prefer to use AbstractPlayer or CNetGamePlayer versions instead of these, because the startup_id here might not match netEndpoint!
		[[nodiscard]] uint32_t getMacId() const;
		[[nodiscard]] netSocketAddress getAddress() const;
		[[nodiscard]] netEndpoint* getEndpoint() const;
		[[nodiscard]] netSocketAddress getEndpointAddress() const;
		[[nodiscard]] netSocketAddress getPublicAddress() const;

		void reset();

		void fromBase64(const std::string& str);
		void fromBinary(const std::string& str);

		/*template <typename T>
		bool ser(T& bb)
		{
			if (bb.serU64(startup_id)
				&& gamer_handle.ser(bb)
				//&& key.ser(bb)
				//&& relay.ser(bb)
				&& online.ser(bb)
				&& lan.ser(bb)
				&& bb.serU32(nat_type, 3)
				//&& relay_token.ser(bb)
				)
			{
				// re-align
				uint32_t dummy = 0;
				bb.serU32(dummy, 8 - 3);

				return true;
			}
			return false;
		}*/
	};
	static_assert(sizeof(netPeerAddress) == 0xC0);
}
#pragma pack(pop)
