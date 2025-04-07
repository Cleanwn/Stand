#pragma once

#include "CommandActionScript.hpp"

#include "AbstractEntity.hpp"
#include "atStringHash.hpp"
#include "lang.hpp"
#include "natives.hpp"
#include "Util.hpp"

namespace Stand
{
	class CommandTune : public CommandActionScript
	{
	public:
		explicit CommandTune(CommandList* const parent)
			: CommandActionScript(parent, LOC("TUNE"), CMDNAMES("tune", "upgrade", "fullupgrade", "fullyupgrade"), LOC("TUNE_H"))
		{
		}

		void onClickScriptThread(Click& click) final
		{
			const Vehicle veh = Util::getVehicle(click);
			if (veh)
			{
				AbstractEntity::get(veh).requestControl(ATSTRINGHASH("tune"), [](AbstractEntity& veh)
				{
					veh.fullyUpgrade();
				}, [](AbstractEntity&)
				{
					Util::toast(LOC("VEHCTRL"));
				});
			}
		}
	};
}
