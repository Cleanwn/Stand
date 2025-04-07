#pragma once

#include "CommandPlayerVehicleAction.hpp"

#include "natives.hpp"

namespace Stand
{
	class CommandPlayerVehKill : public CommandPlayerVehicleAction
	{
	public:
		explicit CommandPlayerVehKill(CommandList* parent)
			: CommandPlayerVehicleAction(parent, LOC("KLL"), { CMDNAME("killveh") }, NOLABEL, PV_CONTROL | PV_ALLOWSELF, COMMANDPERM_AGGRESSIVE)
		{
		}

		void onClickImpl(AbstractPlayer p, AbstractEntity& veh, const Click& click) const final
		{
			VEHICLE::SET_VEHICLE_ENGINE_HEALTH(veh, -4000.0f);
		}
	};
}
