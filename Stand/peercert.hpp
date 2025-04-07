#pragma once

#include "inmap.hpp"
#include "netAddress.hpp"
#include "rlGamerHandle.hpp"

#include "conf.hpp"
#include "main.hpp"

namespace rage
{
	struct netPeerCertificate // made up name
	{
		struct Subject // made up name
		{
			PAD(0x00, 0x08) rlGamerHandle handle;
			/* 0x18 */ char name[17];
			PAD(0x29, 0x48);
		};
		static_assert(sizeof(Subject) == 0x48);
		static_assert(offsetof(Subject, handle) == 0x08);
		static_assert(offsetof(Subject, name) == 0x18);

		/* 0x00 */ inmap_node<netAddress, netPeerCertificate> byAddressLink;
		/* 0x38 */ inmap_node<uint32_t, netPeerCertificate> byKeyLink;
		PAD(0x38 + 0x20, 0x78) netPeerCertificate* next; // likely inlist_node
		/* 0x80 */ netPeerCertificate* prev;
		PAD(0x88, 0x94) uint32_t key;
		PAD(0x98, 0xA0) netAddress address;
		PAD(0xA0 + sizeof(netAddress), 0x188) Subject subject;
		PAD(0x188 + sizeof(Subject), 0x260) uint8_t unk_status_1;
		/* 0x261 */ uint8_t unk_status_2;
	};
	static_assert(offsetof(netPeerCertificate, byAddressLink) == 0x00);
	static_assert(offsetof(netPeerCertificate, byKeyLink) == 0x38);
	static_assert(offsetof(netPeerCertificate, next) == 0x78);
	static_assert(offsetof(netPeerCertificate, key) == 0x94);
	static_assert(offsetof(netPeerCertificate, address) == 0xA0);
	static_assert(offsetof(netPeerCertificate, subject) == 0x188);
	static_assert(offsetof(netPeerCertificate, unk_status_1) == 0x260);
	static_assert(offsetof(netPeerCertificate, unk_status_2) == 0x261);

	/*[[nodiscard]] inline netPeerCertificate* findCertByKey(uint32_t key)
	{
		// 1.66-2824: 00007FF72CE42698
		// 1.69-3258: 00007FF63A8E5638
		return ((inmap<uint32_t, netPeerCertificate, &netPeerCertificate::byKeyLink>*)IDA_ADDR(00007FF63A8E5638))->find(key);
	}*/
}
