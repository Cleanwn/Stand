#include "VehicleMods.hpp"

#include <fmt/core.h>

#include <soup/ObfusString.hpp>

namespace Stand
{
	Label VehicleMods::getName(int modType)
	{
		return LOC_RT(fmt::format(fmt::runtime(soup::ObfusString("VEHMOD{}").str()), modType));
	}
}
