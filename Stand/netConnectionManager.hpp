#pragma once

#include "struct_base.hpp"

#include "atDelegate.hpp"
#include "inlist.hpp"
#include "nettypes.hpp"
#include "netEndpoint.hpp"
#include "sysCriticalSection.hpp"

namespace rage
{
#pragma pack(push, 1)
	template <typename T>
	class netDynArray
	{
	public:
		/* 0x00 */ sysMemAllocator* allocator;
		/* 0x08 */ T** buckets;
		/* 0x10 */ uint32_t size;
		/* 0x14 */ uint32_t capacity;
		/* 0x18 */ uint32_t bucket_size;
		/* 0x1C */ uint32_t bucket_shift;
		/* 0x20 */ uint32_t bucket_mask;

		T& operator[](const unsigned index)
		{
			return buckets[index >> bucket_shift][index & bucket_mask];
		}
	};
	static_assert(sizeof(netDynArray<void*>) == 0x24);
#pragma pack(pop)

	class netConnectionManager
	{
	public:
		class ChannelHandler // made up name
		{
		public:
			/* 0x00 */ void* func_a1;
			/* 0x08 */ void* func;
		};
		static_assert(sizeof(ChannelHandler) == 0x10);

		class Channel
		{
		public:
			uint32_t index;
			PAD(4, 0x40) ChannelHandler* handler;
			PAD(0x48, 0x60)
		};
		static_assert(sizeof(Channel) == 0x60);

		class Delegate : public atDelegate/*<void(netConnectionManager*, const netEvent*)>*/
		{
			/* 0x10 */ netConnectionManager* m_Owner;
			/* 0x18 */ ChannelIds m_ChannelId;
			/* 0x1C */ uint32_t m_ChannelProc;
			/* 0x20 */ inlist_node<Delegate> m_ListLink;
		};
		static_assert(sizeof(Delegate) == 0x30);

		virtual ~netConnectionManager() = default;

		typedef inmap<EndpointId, netEndpoint, &netEndpoint::m_ByEndpointIdLink> EndpointsByEndpointId;
		typedef inmap<netAddress, netEndpoint, &netEndpoint::m_ByAddrLink> EndpointsByAddr;
		typedef inmap<netPeerId, netEndpoint, &netEndpoint::m_ByPeerIdLink> EndpointsByPeerId;

		PAD(0x08, 0x98) EndpointsByEndpointId m_EndpointsByEndpointId;
		//inmap<uint32_t, netEndpoint, &netEndpoint::endpointByCertKeyLink> endpointsByCertKey; // added in 2824
		PAD(0xA8, 0xC8) EndpointsByAddr m_EndpointsByAddr;
		PAD(0xD8, 0xE0) EndpointsByPeerId m_EndpointsByPeerId; // peer id here refers to what I would call the startup id
		PAD(0xF0, 0x110) netDynArray<Cxn> cxns;
		//PAD(0xF8 + sizeof(netDynArray<Cxn>), 0x300) Channel channels[16];
		PAD(0x110 + sizeof(netDynArray<Cxn>), 0x968) sysCriticalSection critical_section;

		[[nodiscard]] netEndpoint* getEndpointById(EndpointId endpoint_id) noexcept;
		[[nodiscard]] netEndpoint* getEndpointByStartupId(netPeerId startup_id) noexcept; // note: may return endpoint where startup_id differs!

		[[nodiscard]] Cxn* GetCxnById(int id) noexcept;

		bool Send(int32_t cxnId, const void* bytes, uint32_t numBytes, uint32_t sendFlags, netSequence* frameSeq = nullptr);

		template <typename T>
		bool Send(int32_t cxnId, const T& msg, uint32_t sendFlags, netSequence* frameSeq = nullptr)
		{
			uint8_t buf[916];
			uint32_t size;
			return msg.Export<T>(buf, sizeof(buf), &size)
				&& this->Send(cxnId, buf, size, sendFlags, frameSeq)
				;
		}

		bool SendOutOfBand(const netAddress& addr, uint32_t channelId, const void* bytes, uint32_t numBytes, uint32_t sendFlags);

		template <typename T>
		bool SendOutOfBand(const netAddress& addr, uint32_t channelId, const T& msg, uint32_t sendFlags)
		{
			uint8_t buf[916];
			uint32_t size;
			return msg.Export<T>(buf, sizeof(buf), &size)
				&& this->SendOutOfBand(addr, channelId, buf, size, sendFlags)
				;
		}
	};
	static_assert(offsetof(netConnectionManager, m_EndpointsByEndpointId) == 0x98); // 3337
	static_assert(offsetof(netConnectionManager, m_EndpointsByAddr) == 0xC8); // 3337
	static_assert(offsetof(netConnectionManager, m_EndpointsByPeerId) == 0xE0); // 3337
	static_assert(offsetof(netConnectionManager, cxns) == 0x110); // 3337
	static_assert(offsetof(netConnectionManager, critical_section) == 0x968); // 3337
}
