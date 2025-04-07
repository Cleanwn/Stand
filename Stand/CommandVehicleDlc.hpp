#pragma once

#include "CommandVehicle.hpp"

#include "StringUtils.hpp"

namespace Stand
{
	class CommandVehicleDlc : public CommandVehicle
	{
	public:
		VehicleItem vitem;

		static void populateVitem(VehicleItem& vitem, const CustomVehicleModel* info)
		{
			vitem.hash_name  = info->hash_name;
			vitem.name = info->name;
			vitem.manufacturer = info->manufacturer;
			vitem.dlc.reset();
		}

		explicit CommandVehicleDlc(CommandList* const parent, const CustomVehicleModel* info)
			: CommandVehicle(parent, (populateVitem(vitem, info), &vitem))
		{
			StringUtils::simplifyCommandName(this->command_names.at(0));
		}
	};
}
