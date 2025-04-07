#pragma once

#include "netPeerAddress.hpp"
#include "netSocketAddress.hpp"

namespace rage
{
	struct netIceSessionOffer
	{
		/* 0x000 */ uint8_t m_Version;
		/* 0x001 */ uint8_t m_NumCandidates;
		/* 0x002 */ bool m_Bilateral;
		/* 0x003 */ bool m_SendDebugInfo;
		/* 0x004 */ bool m_NonDeterministic;
		/* 0x008 */ uint32_t m_SourceSessionId;
		/* 0x00C */ uint32_t m_DestSessionId;
		/* 0x010 */ netPeerAddress m_SourcePeerAddr;
		/* 0x0D0 */ netPeerAddress m_DestPeerAddr;
		/* 0x190 */ netSocketAddress m_Candidates[8];
		char please_dont_buffer_overflow_me_daddy_rockstar[0x600];
	};
	static_assert(offsetof(netIceSessionOffer, m_DestPeerAddr) == 0xD0);
	static_assert(offsetof(netIceSessionOffer, m_Candidates) == 0x190);

	struct netIceSessionAnswer
	{
		/* 0x00 */ uint8_t m_Version;
		/* 0x01 */ uint8_t m_NumCandidates;
		/* 0x02 */ bool m_SenderHasControllingRole;
		/* 0x03 */ bool m_SendDebugInfo;
		/* 0x04 */ bool m_NonDeterministic;
		/* 0x08 */ uint32_t m_SourceSessionId;
		/* 0x0C */ uint32_t m_DestSessionId;
		/* 0x10 */ uint32_t m_TransactionId;
		PAD(0x14, 0x18) netPeerAddress m_SourcePeerAddr;
		/* 0xD8 */ netSocketAddress m_Candidates[8];
	};
	static_assert(offsetof(netIceSessionAnswer, m_TransactionId) == 0x10);
	static_assert(offsetof(netIceSessionAnswer, m_SourcePeerAddr) == 0x18);
	static_assert(offsetof(netIceSessionAnswer, m_Candidates) == 0xD8);
}
