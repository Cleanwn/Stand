#pragma once

#include "struct_base.hpp"

#include "inmap.hpp"
#include "net.hpp"
#include "netAddress.hpp"
#include "netConnection.hpp"
#include "netPeerAddress.hpp"
#include "netStatus.hpp"

namespace rage
{
	class netEndpoint
	{
	public:
		typedef inlist<Cxn, &Cxn::m_ListLink> CxnList;
		//typedef inlist<CxnlessBundle, &CxnlessBundle::m_ListLink> CxnlessQueue;

		/* 0x000 */ Cxn* channel_cxns[NET_MAX_CHANNELS];
		/* 0x080 */ CxnList m_CxnList;
		PAD(0x098, 0x0B0); // CxnlessQueue m_CxnlessQ;
		/* 0x0B0 */ void* m_TunnelRqst;
		PAD(0x0B8, 0x0BC); // added in 2824, seems to always be 1
		/* 0x0BC */ netStatus m_myStatus;
		/* 0x0C8 */ netStatus* m_Status;
		/* 0x0D0 */ netAddress relay_address;
		/* 0x0F0 */ netPeerAddress peer_address; // 2824: 0xE8 -> 0xF0 (+0x08)
		/* 0x1B0 */ inmap_node<EndpointId, netEndpoint> m_ByEndpointIdLink; // 2824: 0x148 -> 0x1B0 (+0x68)
		/* 0x1D0 */ inmap_node<uint32_t, netEndpoint> endpointByCertKeyLink; // added in 2824
		/* 0x1F0 */ inmap_node<netPeerId, netEndpoint> m_ByPeerIdLink; // peer id here refers to what I would call the startup id
		/* 0x210 */ inmap_node<netAddress, netEndpoint> m_ByAddrLink; // correct 3095
		PAD(0x248, 0x258); // added in 2824, seems like two netEndpoint pointers, likely inlist_node
		/* 0x258 */ int m_PackTimeout;
		/* 0x25C */ int m_SendInterval;
		/* 0x260 */ int m_Timeout;
		/* 0x264 */ unsigned int m_LastTime;
		/* 0x268 */ unsigned m_LastReceiveTime;
		/* 0x26C */ unsigned m_CreatedTime;
		/* 0x270 */ unsigned m_LastAddressAssignmentTime;
		void* m_EventQ;
		netEventOutOfMemory m_OutOfMemEvent;
		/* 0x2E0 */ EndpointId id;

		// peer id here refers to what I would call the startup id
		[[nodiscard]] const netPeerId& GetPeerId() const
		{
			return m_ByPeerIdLink.m_key;
		}

		[[nodiscard]] const netAddress& GetAddress() const
		{
			return m_ByAddrLink.m_key;
		}

		[[nodiscard]] uint32_t getCertKey() const
		{
			return endpointByCertKeyLink.m_key;
		}

		// Result's ip field will be -1 if address is not TYPE_DIRECT
		[[nodiscard]] const netSocketAddress& getRemoteAddress() const
		{
#ifdef STAND_DEBUG
			sanityCheck();
#endif
			return GetAddress().sock_addr;
		}

		[[nodiscard]] const netSocketAddress& getPublicAddress() const;

#ifdef STAND_DEBUG
		void sanityCheck() const;
#endif

		void removeGamer(uint32_t channelId, uint64_t sessionId, const rlGamerHandle& target, int remove_reason = -1) const;
	};
	//static_assert(sizeof(netEndpoint) == 0x2E0 + sizeof(EndpointId));
	static_assert(offsetof(netEndpoint, m_TunnelRqst) == 0x0B0);
	static_assert(offsetof(netEndpoint, m_myStatus) == 0x0BC);
	static_assert(offsetof(netEndpoint, m_Status) == 0x0C8);
	static_assert(offsetof(netEndpoint, m_ByEndpointIdLink) == 0x1B0);
	static_assert(offsetof(netEndpoint, endpointByCertKeyLink.m_key) == 0x1E8);
	static_assert(offsetof(netEndpoint, m_ByPeerIdLink) == 0x1F0);
	static_assert(offsetof(netEndpoint, m_ByPeerIdLink.m_key) == 0x208);
	static_assert(offsetof(netEndpoint, m_ByAddrLink.m_key) == 0x228); // 2824: 0x1A0 -> 0x228 (+0x88)
	static_assert(offsetof(netEndpoint, m_PackTimeout) == 0x258);
	static_assert(offsetof(netEndpoint, m_LastTime) == 0x264);
	static_assert(offsetof(netEndpoint, m_CreatedTime) == 0x26C);
	static_assert(offsetof(netEndpoint, m_LastAddressAssignmentTime) == 0x270);
	static_assert(offsetof(netEndpoint, id) == 0x2E0);
}
