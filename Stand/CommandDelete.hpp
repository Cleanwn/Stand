#pragma once

#include "CommandActionScript.hpp"

#include "atStringHash.hpp"
#include "natives.hpp"

namespace Stand
{
	class CommandDelete : public CommandActionScript
	{
	public:
		explicit CommandDelete(CommandList* const parent)
			: CommandActionScript(parent, LOC("DEL"), CMDNAMES("deletevehicle"), LOC("DELVEH_H"))
		{
		}

		void onClickScriptThread(Click& click) final
		{
			if (auto veh = Util::getVehicle(click); veh.isValid())
			{
				veh.requestControl(ATSTRINGHASH("delete"), [](AbstractEntity& veh)
				{
					veh.removeFromPlaneOfExistence();
				}, [](AbstractEntity&)
				{
					Util::toast(LOC("VEHCTRL"));
				});
			}
		}
	};
}
