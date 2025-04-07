#pragma once

#include "CommandPlayerVehicleAction.hpp"

#include "natives.hpp"

namespace Stand
{
	class CommandPlayerVehRemoveDoors : public CommandPlayerVehicleAction
	{
	public:
		explicit CommandPlayerVehRemoveDoors(CommandList* parent)
			: CommandPlayerVehicleAction(parent, LOC("DEL_DOORS"), { CMDNAME("removedoors") }, NOLABEL, PV_CONTROL | PV_ALLOWSELF, COMMANDPERM_AGGRESSIVE)
		{
		}

		void onClickImpl(AbstractPlayer p, AbstractEntity& veh, const Click& click) const final
		{
			for (uint8_t i = 0; i != 7; ++i)
			{
				VEHICLE::SET_VEHICLE_DOOR_BROKEN(veh, i, false);
			}
		}
	};
}
