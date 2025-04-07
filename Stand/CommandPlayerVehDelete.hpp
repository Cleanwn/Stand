#pragma once

#include "CommandPlayerVehicleAction.hpp"

namespace Stand
{
	class CommandPlayerVehDelete : public CommandPlayerVehicleAction
	{
	public:
		explicit CommandPlayerVehDelete(CommandList* parent)
			: CommandPlayerVehicleAction(parent, LOC("DEL"), { CMDNAME("delveh") }, NOLABEL, PV_CONTROL | PV_ALLOWSELF, COMMANDPERM_AGGRESSIVE)
		{
		}

		void onClickImpl(AbstractPlayer p, AbstractEntity& veh, const Click& click) const final
		{
			veh.removeFromPlaneOfExistence();
		}
	};
}
