#pragma once

#include "CommandOnPlayerVehicle.hpp"
#include "CommandSliderFloat.hpp"

namespace Stand
{
	class CommandPlayerVehEnginePowerMultiplier : public CommandOnPlayerVehicle<CommandSliderFloat>
	{
	public:
		explicit CommandPlayerVehEnginePowerMultiplier(CommandList* const parent)
			: CommandOnPlayerVehicle(parent, LOC("PLYENGMULT"), CMDNAMES_OBF("givepower"), NOLABEL, 100, 2000, 100, 50, CMDFLAGS_SLIDER, COMMANDPERM_FRIENDLY, true)
		{
			this->pv_flags = PV_CONTROL | PV_ALLOWSELF;
			PC_PTR->registerCommand(this);
		}

		void onClickImpl(AbstractPlayer p, AbstractEntity& veh, const Click& click) const final
		{
			VEHICLE::MODIFY_VEHICLE_TOP_SPEED(veh, getFloatValue() * 10.0f);
		}
	};
}