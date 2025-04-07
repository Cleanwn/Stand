#pragma once

#include "gta_fwddecl.hpp"

#include "net.hpp"
#include "netConnectionManager.hpp"

namespace rage
{
	struct netTimeSync
	{
		virtual ~netTimeSync() = default;

		/* 0x08 */ netConnectionManager* m_CxnMgr;
		/* 0x10 */ EndpointId m_ServerEndpointId;
		/* 0x14 */ uint32_t m_Token; // unique token to identify this clock
		/* 0x18 */ uint32_t m_ServerTimeOffset;
		/* 0x20 */ netConnectionManager::Delegate m_Dlgt;
		/* 0x50 */ uint32_t m_NextPingTime;
		/* 0x54 */ uint32_t m_InitialPingInterval;
		/* 0x58 */ uint32_t m_FinalPingInterval;
		/* 0x5C */ uint32_t m_PingInterval;
		/* 0x60 */ uint32_t m_AverageRTT;
		/* 0x64 */ uint32_t m_NumRejectedPongs;
		/* 0x68 */ uint32_t m_SentSeq;
		/* 0x6C */ uint32_t m_RcvdSeq;
		/* 0x70 */ uint32_t m_Flags; // 1 = server, 2 = client
		/* 0x74 */ uint32_t m_ChannelId;
		/* 0x78 */ uint32_t m_LastStableTime;
		/* 0x7C */ uint8_t sync_flags; // 1 = has synched, 2 = has stable time
	};
	static_assert(sizeof(netTimeSync) == 0x80);
	static_assert(offsetof(netTimeSync, m_ServerTimeOffset) == 0x18);
	static_assert(offsetof(netTimeSync, m_Dlgt) == 0x20);
	static_assert(offsetof(netTimeSync, m_NextPingTime) == 0x50);
	static_assert(offsetof(netTimeSync, m_SentSeq) == 0x68);

	struct netTimeSyncMsg
	{
		enum Type
		{
			TYPE_PING = 0,
			TYPE_PONG = 1,
		};

		uint32_t m_Type;
		uint32_t m_Seq;
		uint32_t m_Token;
		uint32_t m_PingSendTime;
		uint32_t m_PingRecvTime; // only present in PONG messages
	};
	static_assert(sizeof(netTimeSyncMsg) == 0x14);
}
