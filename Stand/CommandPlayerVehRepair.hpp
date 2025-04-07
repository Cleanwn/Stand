#pragma once

#include "CommandPlayerAction.hpp"

namespace Stand
{
	class CommandPlayerVehRepair : public CommandPlayerAction
	{
	public:
		explicit CommandPlayerVehRepair(CommandList* parent)
			: CommandPlayerAction(parent, LOC("FIXVEH"), CMDNAMES_OBF("repairveh"), NOLABEL, COMMANDPERM_FRIENDLY)
		{
		}

		void onClick(Click& click) final
		{
			for (const auto& p : PP_PTR->getPlayers())
			{
				p.giveVehicleRepair();
			}
		}
	};
}
