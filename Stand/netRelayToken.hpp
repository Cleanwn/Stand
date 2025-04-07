#pragma once

#pragma pack(push, 1)
namespace rage
{
	struct netRelayToken
	{
		char data[6];

		template <typename T>
		bool ser(T& bb)
		{
			return bb.serBytes(data, sizeof(data));
		}
	};
	static_assert(sizeof(netRelayToken) == 6);
}
#pragma pack(pop)
