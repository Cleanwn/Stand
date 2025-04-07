#pragma once

#include "CommandPlayerVehicleAction.hpp"

namespace Stand
{
	class CommandPlayerVehFlip : public CommandPlayerVehicleAction
	{
	public:
		explicit CommandPlayerVehFlip(CommandList* parent)
			: CommandPlayerVehicleAction(parent, LOC("FLIPPLRVEH"), CMDNAMES_OBF("flipveh"), NOLABEL, PV_CONTROL | PV_ALLOWSELF, COMMANDPERM_AGGRESSIVE)
		{
		}

		void onClickImpl(AbstractPlayer p, AbstractEntity& veh, const Click& click) const final
		{
			auto rot = veh.getRot();
			rot.y -= 180.0f;
			veh.setRot(rot);
		}
	};
}
