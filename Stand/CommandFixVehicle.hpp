#pragma once

#include "CommandActionScript.hpp"

namespace Stand
{
	class CommandFixVehicle : public CommandActionScript
	{
	public:
		explicit CommandFixVehicle(CommandList* const parent)
			: CommandActionScript(parent, LOC("FIXVEH"), { CMDNAME("fixvehicle"), CMDNAME("repairvehicle") }, LOC("FIXVEH_H"), CMDFLAGS_ACTION, COMMANDPERM_FRIENDLY)
		{
		}

		void onClickScriptThread(Click& click) final
		{
			if (click.getEffectiveIssuer() == g_player)
			{
				if (auto veh = Util::getVehicle(click, true); veh.isValid())
				{
					auto driver = veh.getDriver().getPlayer();
					if (driver.isValid()
						&& driver != g_player
						)
					{
						driver.giveVehicleRepair();
					}
					else
					{
						veh.repair();
						veh.wash();
					}
				}
			}
			else
			{
				click.getEffectiveIssuer().giveVehicleRepair();
			}
		}
	};
}
