#pragma once

#include "CommandPlayerVehicleAction.hpp"

#include "natives.hpp"

namespace Stand
{
	class CommandPlayerVehIgnite : public CommandPlayerVehicleAction
	{
	public:
		explicit CommandPlayerVehIgnite(CommandList* parent)
			: CommandPlayerVehicleAction(parent, LOC("IGNITE"), { CMDNAME("igniteveh") }, NOLABEL, PV_CONTROL | PV_ALLOWSELF, COMMANDPERM_AGGRESSIVE)
		{
		}

		void onClickImpl(AbstractPlayer p, AbstractEntity& veh, const Click& click) const final
		{
			VEHICLE::SET_VEHICLE_PETROL_TANK_HEALTH(veh, -1.0f);
		}
	};
}
