#pragma once

#include "CommandPlayerVehicleAction.hpp"

namespace Stand
{
	class CommandPlayerVehDowngrade : public CommandPlayerVehicleAction
	{
	public:
		explicit CommandPlayerVehDowngrade(CommandList* parent)
			: CommandPlayerVehicleAction(parent, LOC("DNGRD"), CMDNAMES("vehdowngrade"), NOLABEL, PV_CONTROL | PV_ALLOWSELF, COMMANDPERM_AGGRESSIVE)
		{
		}

		void onClickImpl(AbstractPlayer p, AbstractEntity& veh, const Click& click) const final
		{
			veh.downgrade();
		}
	};
}
