#pragma once

#include <cstdint>

namespace rage
{
	struct netHardware
	{
		static void GetMacAddress(uint64_t* mac_address);
	};
}
