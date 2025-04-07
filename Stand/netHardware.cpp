#include "netHardware.hpp"

#include <Rpc.h>
#pragma comment(lib, "rpcrt4.lib")

namespace rage
{
	void netHardware::GetMacAddress(uint64_t* mac_address)
	{
		UUID uuid;
		UuidCreateSequential(&uuid);
		*(uint32_t*)mac_address = *(uint32_t*)&uuid.Data4[2];
		((uint16_t*)mac_address)[2] = *(uint16_t*)&uuid.Data4[6];
		((uint16_t*)mac_address)[3] = 0;
	}
}
