#pragma once

#include "struct_base.hpp"

#include "conf_netcode.hpp"

#include "inlist.hpp"
#include "netEvent.hpp"

namespace rage
{
	class netConnection
	{
	public:
		enum State
		{
			STATE_LISTEN,
			STATE_SYN_SENT,
			STATE_SYN_RECEIVED,
			STATE_ESTABLISHED,
			STATE_CLOSED,
		};

		class InFrame : public netEventFrameReceived
		{
		};

		virtual ~netConnection() = default;

		/* 0x08 */ netEndpoint* endpoint;
		/* 0x10 */ int32_t id;
		/* 0x14 */ uint32_t channel_index;
		/* 0x18 */ sysMemAllocator* m_Allocator;
		/* 0x20 */ uint32_t state;
		/* 0x24 */ uint32_t m_LastKeepaliveTime;
		/* 0x28 */ uint32_t m_LastReceiveTime;
		/* 0x30 */ int32_t m_TimeoutCount;
		/* 0x34 */ uint32_t m_RttOverThresholdStartTime;
		/* 0x38 */ uint32_t m_RttOverThresholdLastTime;
		PAD(0x3C, 0xB0) rage::netConnectionManager* m_CxnMgr;
		PAD(0xB8, 0xD8)

#if I_CAN_SPY
		bool QueueFrame(const void* bytes, uint32_t numBytes, uint32_t sendFlags, netSequence* frameSeq);
#endif
	};
	static_assert(sizeof(netConnection) == 0xD8);

	class Cxn : public netConnection
	{
	public:
		/* 0xD8 */ inlist_node<Cxn> m_ListLink;
		/* 0xE8 */ uint32_t m_Refs;
		/* 0xF0 */ netStatus* m_Status; // Pointer to a status object owned by the caller of OpenConnection.		
		/* 0xF8 */ netEventConnectionClosed m_CxnClosedEvent; // Pre-allocated connection closed event.
		/* 0x150 */ netEventOutOfMemory m_OutOfMemEvent; // Pre-allocated out of memory event.
		/* 0x1B0 */ bool m_PendingClose : 1;
		/* 0x1B4 */ uint32_t m_PendingCloseTime;

		[[nodiscard]] bool IsOpen() const noexcept
		{
			return !IsPendingClose()
				&& state == STATE_ESTABLISHED
				;
		}

		[[nodiscard]] bool IsPendingClose() const noexcept
		{
			return m_PendingClose;
		}
	};
	static_assert(sizeof(Cxn) == 0x1B8);
}