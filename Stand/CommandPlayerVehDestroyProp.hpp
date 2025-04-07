#pragma once

#include "CommandPlayerVehicleAction.hpp"

#include "natives.hpp"

namespace Stand
{
	class CommandPlayerVehDestroyProp : public CommandPlayerVehicleAction
	{
	public:
		explicit CommandPlayerVehDestroyProp(CommandList* parent)
			: CommandPlayerVehicleAction(parent, LOC("DSTRYPROP"), { CMDNAME("destroyprop") }, NOLABEL, PV_CONTROL | PV_ALLOWSELF, COMMANDPERM_AGGRESSIVE)
		{
		}

		void onClickImpl(AbstractPlayer p, AbstractEntity& veh, const Click& click) const final
		{
			const auto model = veh.getModel();

			if (model.isHeli() || model.isPlane())
			{
				VEHICLE::SET_PLANE_PROPELLER_HEALTH(veh, -100.0f);
				VEHICLE::SET_HELI_TAIL_ROTOR_HEALTH(veh, -100.0f);
			}
			else
			{
				click.setResponse(LOC("GENFAIL"));
			}
		}
	};
}
