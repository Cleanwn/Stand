#pragma once

#include "CommandPlayerVehicleAction.hpp"

namespace Stand
{
	class CommandPlayerVehicleUpgrade : public CommandPlayerVehicleAction
	{
	public:
		explicit CommandPlayerVehicleUpgrade(CommandList* const parent)
			: CommandPlayerVehicleAction(parent, LOC("TUNE"), CMDNAMES_OBF("upgradeveh"), NOLABEL, PV_CONTROL | PV_ALLOWSELF, COMMANDPERM_FRIENDLY)
		{
		}

		void onClickImpl(AbstractPlayer p, AbstractEntity& veh, const Click& click) const final
		{
			veh.fullyUpgrade();
		}
	};
}