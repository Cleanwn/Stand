#pragma once

#include "struct_base.hpp"

#include "netAddress.hpp"

namespace rage
{
	class ReceivedMessageData
	{
	public:
		void* m_bitbuffer_data; // 0x0
		uint32_t m_bitbuffer_size; // 0x8
		char pad_0x8[0x4]; // 0xC
		CNetGamePlayer* sender; // 0x10 // for net game and script handler events
		CNetGamePlayer* receiver; // 0x18 // for net game events
	};
	static_assert(sizeof(ReceivedMessageData) == 0x20);

	class netEventBase
	{
	public:
		virtual ~netEventBase() = default;
	};
	static_assert(sizeof(netEventBase) == 8);

	enum eNetEventId : uint32_t
	{
		//A connection was requested from a remote endpoint.
		NET_EVENT_CONNECTION_REQUESTED,
		//A connection was established.
		NET_EVENT_CONNECTION_ESTABLISHED,
		//There was an error on the connection.
		NET_EVENT_CONNECTION_ERROR,
		//A connection was closed.
		NET_EVENT_CONNECTION_CLOSED,
		//A frame of data was received.
		NET_EVENT_FRAME_RECEIVED,
		//An ack for a previously sent frame was received.
		NET_EVENT_ACK_RECEIVED,
		//Bandwidth limit was exceeded.
		NET_EVENT_BANDWIDTH_EXCEEDED,
		//Ran out of memory.
		NET_EVENT_OUT_OF_MEMORY
	};

	class netEvent : public netEventBase
	{
	public:
		virtual ~netEvent() = default;
		virtual void* _0x08() { return nullptr; }
		virtual eNetEventId GetId() const = 0;

		PAD(8, 0x40) uint32_t con_id; // not applicable for out-of-band messages?
		/* 0x44 */ uint32_t channel_id;
		/* 0x48 */ class netEvent* unk_thisptr; // correct 2824
		/* 0x50 */ uint32_t endpoint_id;
		PAD(0x54, 0x58)
	};
	static_assert(sizeof(netEvent) == 0x58);

	class netEventConnectionRequested : public netEvent
	{
	public:
		/* 0x58 */ netAddress sender;
	};

	class netEventConnectionClosed : public netEvent
	{
	public:
		eNetEventId GetId() const final { return NET_EVENT_CONNECTION_CLOSED; }
	};

	class netEventFrameReceived : public netEvent
	{
	public:
		eNetEventId GetId() const final { return NET_EVENT_FRAME_RECEIVED; }

		/* 0x58 */ uint32_t cert_key;
		PAD(0x5C, 0x60) netAddress sender;
		/* 0x80 */ uint32_t size;
		/* 0x88 */ ReceivedMessageData* data;
	};
	static_assert(sizeof(netEventFrameReceived) == 0x88 + 8);
	static_assert(offsetof(netEventFrameReceived, sender) == 0x60);
	static_assert(offsetof(netEventFrameReceived, size) == 0x80);
	static_assert(offsetof(netEventFrameReceived, data) == 0x88);

	class netEventOutOfMemory : public netEvent
	{
	public:
		eNetEventId GetId() const final { return NET_EVENT_OUT_OF_MEMORY; }

		bool m_isFatal;
	};
}
