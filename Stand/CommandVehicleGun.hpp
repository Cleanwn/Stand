#pragma once

#include "CommandList.hpp"

#include "CommandVehicleGunToggle.hpp"

namespace Stand
{
	class CommandVehicleGun : public CommandList
	{
	public:
		explicit CommandVehicleGun(CommandList* const parent)
			: CommandList(parent, LOC("VEHGUN"), CMDNAMES_OBF("vehiclegun"), LOC("VEHGUN_H"))
		{
			auto toggle = createChild<CommandVehicleGunToggle>();

			toggle->model = createChild<CommandListSelectVehicleModel>(LOC("VEH"), {}, NOLABEL, CommandListSelectVehicleModel::RANDOM, true, false, std::vector<hash_t>{ ATSTRINGHASH("rail") }, CommandListSelectVehicleModel::Disallows::BIG_VEHICLES);
			toggle->speed = createChild<CommandSlider>(LOC("SPD"), CMDNAMES_OBF("vehiclegunspeed"), NOLABEL, 20, 200, 200);
			toggle->distance = createChild<CommandSlider>(LOC("DIST"), CMDNAMES_OBF("vehiclegundistance"), NOLABEL, 10, 30, 10);
		}
	};
}