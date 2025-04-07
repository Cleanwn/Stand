#pragma once

#include "struct_base.hpp"

#include "netPeerId.hpp"
#include "netRelayToken.hpp"
#include "netSocketAddress.hpp"

#pragma pack(push, 1)
namespace rage
{
	class netAddress
	{
	public:
		enum Type : uint8_t
		{
			TYPE_INVALID,
			TYPE_DIRECT,
			TYPE_RELAY_SERVER,
			TYPE_PEER_RELAY,

			NUM_TYPES
		};

		/* 0x00 */ netSocketAddress proxy_sock_addr;
		/* 0x08 */ netSocketAddress sock_addr;
		/* 0x10 */ netPeerId peerId;
		/* 0x18 */ netRelayToken m_RelayToken;
		/* 0x1E */ uint8_t m_Type;
		PAD(0x1F, 0x20);

		netAddress() = default;

		netAddress(const netSocketAddress& relay_addr, const netSocketAddress& target_addr, const netRelayToken& relay_token)
			: m_Type(TYPE_PEER_RELAY), proxy_sock_addr(relay_addr), sock_addr(target_addr), m_RelayToken(relay_token)
		{
		}

		bool operator<(const netAddress& rhs) const;

		/*template <typename T>
		bool ser(T& bb)
		{
			if (bb.serU8(m_Type)
				&& proxy_sock_addr.ser(bb)
				)
			{
				if (m_Type == TYPE_RELAY_SERVER)
				{
					return m_RelayToken.ser(bb)
						&& sock_addr.ser(bb)
						;
				}
				else if (m_Type == TYPE_PEER_RELAY)
				{
					return bb.serU64(peerId);
				}
				else
				{
					return sock_addr.ser(bb);
				}
			}
			return false;
		}*/
	};
	static_assert(sizeof(netAddress) == 0x20);
}
#pragma pack(pop)
