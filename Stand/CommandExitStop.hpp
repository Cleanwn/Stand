#pragma once

#include "CommandToggle.hpp"

#include "AbstractEntity.hpp"
#include "natives.hpp"

namespace Stand
{
	class CommandExitStop : public CommandToggle
	{
	public:
		explicit CommandExitStop(CommandList* const parent)
			: CommandToggle(parent, LOC("EXITSTOP"), { CMDNAME("exitstop") })
		{
		}

		void onChange(Click& click) final
		{
			onChangeToggleScriptTickEventHandler(click, []()
			{
				if (g_player_veh.isValid() && TASK::GET_IS_TASK_ACTIVE(g_player_ped, CTaskTypes::TASK_EXIT_VEHICLE) && g_player_veh.isOwner())
				{
					ENTITY::SET_ENTITY_VELOCITY(g_player_veh, 0.0f, 0.0f, 0.0f);
					VEHICLE::SET_ROCKET_BOOST_ACTIVE(g_player_veh, FALSE);
				}
			});
		}
	};
}
