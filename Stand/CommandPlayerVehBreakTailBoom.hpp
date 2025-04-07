#pragma once

#include "CommandPlayerVehicleAction.hpp"

#include "AbstractModel.hpp"
#include "pointers.hpp"

namespace Stand
{
	class CommandPlayerVehBreakTailBoom : public CommandPlayerVehicleAction
	{
	public:
		explicit CommandPlayerVehBreakTailBoom(CommandList* parent)
			: CommandPlayerVehicleAction(parent, LOC("BREAKBOOM"), CMDNAMES_OBF("breakofftailboom"), NOLABEL, PV_CONTROL | PV_ALLOWSELF, COMMANDPERM_AGGRESSIVE)
		{
		}

		void onClickImpl(AbstractPlayer p, AbstractEntity& veh, const Click& click) const final
		{
			const auto model = veh.getModel();

			if (model.isHeli())
			{
				pointers::CRotaryWingAircraft_BreakOffTailBoom((CRotaryWingAircraft*)veh.getPointer(), 0);
			}
			else
			{
				click.setResponse(LOC("GENFAIL"));
			}
		}
	};
}
