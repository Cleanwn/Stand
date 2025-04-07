#pragma once

#include "netStatus.hpp"
#include "rlGamerHandle.hpp"
#include "sc_presence_attributes.hpp"

namespace rage
{
	enum netTunnelType : int8_t
	{
		NET_TUNNELTYPE_INVALID = -1,
		NET_TUNNELTYPE_LAN,
		NET_TUNNELTYPE_ONLINE
	};

	enum netTunnelReason : int8_t
	{
		NET_TUNNEL_REASON_P2P = 4,
	};

	struct netTunnelDesc
	{
		/* 0x00 */ netTunnelType m_TunnelType = NET_TUNNELTYPE_ONLINE;
		/* 0x01 */ netTunnelReason m_TunnelReason = NET_TUNNEL_REASON_P2P;
		/* 0x08 */ uint64_t m_UniqueId; // session token
		/* 0x10 */ bool m_Bilateral; // !isHost
		/* 0x11 */ bool unk = true;

		netTunnelDesc() = default;

		netTunnelDesc(uint64_t token)
			: m_UniqueId(token), m_Bilateral(true)
		{
		}
	};
	static_assert(offsetof(netTunnelDesc, m_TunnelReason) == 1);

	struct netTunnelRequest
	{
		/* 0x00 */ unsigned m_Id = 0;
		/* 0x04 */ /*State*/ int m_State = 0;
		/* 0x08 */ netStatus* m_Status = nullptr;

		/* 0x10 */ EndpointId m_EndpointId = -1;
		/* 0x18 */ rlGamerHandle gamer_handle{};
		/* 0x28 */ netPeerAddress m_PeerAddr{};
		/* 0xE8 */ netAddress m_QuickConnectDirectAddr{};
		/* 0x108 */ netAddress m_NetAddr{};
		/* 0x128 */ netAddress m_RelayAddr{};
		/* 0x148 */ netAddress m_PrevRelayAddress{};
		/* 0x168 */ netAddress unkaddr{};
		/* 0x188 */ netTunnelDesc m_TunnelDesc{};

		/* 0x1A0 */ inlist_node<netTunnelRequest> m_ListLink;
		/* 0x1B0 */ /*netTunneler*/void* m_Tunneler = nullptr;

		rlScPresenceAttribute m_PresenceAttrs[1];
		netStatus m_DirectTunnelingStatus{};
		netStatus m_RelayDiscoveryStatus{};
		netStatus m_RelayConnectionStatus{};
		netStatus m_KeyExchangeStatus{};

		/*PresenceQueryState*/ int m_PresenceQueryState = 0;
		bool m_ForcePresenceQuery = false;
		int m_DirectResultCode = 0;
		int m_KeyExchangeResultCode = 0;
		unsigned m_TimeStarted = 0;
		unsigned m_TimeStateStarted = 0;
	};
	static_assert(offsetof(netTunnelRequest, gamer_handle) == 0x18);
	static_assert(offsetof(netTunnelRequest, m_PeerAddr) == 0x28);
	static_assert(offsetof(netTunnelRequest, m_TunnelDesc) == 0x188);
	static_assert(offsetof(netTunnelRequest, m_ListLink) == 0x1A0);
	static_assert(offsetof(netTunnelRequest, m_Tunneler) == 0x1B0);
}
