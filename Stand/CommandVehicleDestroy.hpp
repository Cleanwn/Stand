#pragma once

#include "CommandActionScript.hpp"

#include "AbstractEntity.hpp"
#include "Util.hpp"

namespace Stand
{
	class CommandVehicleDestroy : public CommandActionScript
	{
	public:
		explicit CommandVehicleDestroy(CommandList* const parent)
			: CommandActionScript(parent, LOC("DSTRY"), { CMDNAME("destroyvehicle") })
		{
		}

		void onClickScriptThread(Click& click) final
		{
			auto veh = Util::getVehicle(click);
			if (veh.isValid())
			{
				veh.destroy();
			}
		}
	};
}
