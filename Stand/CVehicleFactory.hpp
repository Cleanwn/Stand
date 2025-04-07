#pragma once

#include "gta_fwddecl.hpp"

class CVehicleFactory
{
public:
	virtual ~CVehicleFactory() = default;
	virtual void _0x08() = 0;
	virtual void _0x10() = 0;
	virtual void _0x18() = 0;
	virtual void _0x20() = 0;
	virtual void _0x28() = 0;
	virtual void deleteVehicle(CVehicle*, bool) = 0;
};
