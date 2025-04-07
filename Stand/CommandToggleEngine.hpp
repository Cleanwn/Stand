#pragma once

#include "CommandAction.hpp"

#include "natives.hpp"
#include "Util.hpp"

namespace Stand
{
	class CommandToggleEngine : public CommandAction
	{
	public:
		explicit CommandToggleEngine(CommandList* const parent)
			: CommandAction(parent, LOC("ENGTGL"), { CMDNAME("toggleengine"), CMDNAME("togglengine") }, LOC("ENGTGL_H"))
		{
		}

		void onClick(Click& click) final
		{
			if (auto veh = Util::getVehicle(click); veh.isValid())
			{
				if (VEHICLE::GET_IS_VEHICLE_ENGINE_RUNNING(g_player_veh))
				{
					VEHICLE::SET_VEHICLE_ENGINE_ON(veh, false, true, true);
				}
				else
				{
					VEHICLE::SET_VEHICLE_ENGINE_ON(veh, true, true, false);
				}
			}
		}
	};
}