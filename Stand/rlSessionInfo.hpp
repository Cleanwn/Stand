#pragma once

#include "netPeerAddress.hpp"

namespace Stand
{
	struct SessionId
	{
		uint64_t id;
		uint64_t token;
		int64_t host_rid;
	};
	static_assert(sizeof(SessionId) == 8 * 3);
}

namespace rage
{
#pragma pack(push, 1)
	struct rlSessionInfo
	{
		uint64_t id{};
		uint64_t token{};
		netPeerAddress host{};

		rlSessionInfo() = default;
		rlSessionInfo(const std::string& gsinfo);

		void fromGsInfo(const std::string& gsinfo);

		void fromBinary(void* data, uint32_t size_bytes);

		void reset();

		[[nodiscard]] Stand::SessionId getId() const;
		[[nodiscard]] uhash_t getIdHash() const;

		[[nodiscard]] std::string toGsinfo() const;
		[[nodiscard]] std::string toBase64() const;

		/*template <typename T>
		bool ser(T& bb)
		{
			return host.ser(bb)
				&& bb.serU64(id)
				&& bb.serU64(token)
				;
		}*/
	};
	static_assert(sizeof(rlSessionInfo) == 0xD0);
#pragma pack(pop)
}
