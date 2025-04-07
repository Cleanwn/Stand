#pragma once

#include "netIpAddress.hpp"

#pragma pack(push, 1)
namespace rage
{
	class netSocketAddress
	{
	public:
		netIpAddress ip{};
		soup::native_u16_t port = 0;
	private:
		uint16_t pad;

	public:
		[[nodiscard]] bool operator==(const netSocketAddress& b) const noexcept
		{
			return ip == b.ip && port == b.port;
		}

		template <typename T>
		bool ser(T& bb)
		{
			return bb.serU32(ip.value)
				&& bb.serU16(port)
				;
		}
	};
	static_assert(sizeof(netSocketAddress) == 8);
}
#pragma pack(pop)
