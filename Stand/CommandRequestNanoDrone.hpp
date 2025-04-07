#pragma once

#include "CommandActionScript.hpp"

#include "AbstractEntity.hpp"
#include "AbstractPlayer.hpp"
#include "ScriptGlobal.hpp"

namespace Stand
{
	class CommandRequestNanoDrone : public CommandActionScript
	{
	public:
		explicit CommandRequestNanoDrone(CommandList* const parent)
			: CommandActionScript(parent, LOC("REQNANO"), CMDNAMES_OBF("requestnanodrone", "nanodrone"))
		{
		}

		void onClickScriptThread(Click& click) final
		{
			if (!click.inOnline())
			{
				return;
			}
			else if (g_player.isInInterior()
				|| ENTITY::GET_ENTITY_SUBMERGED_LEVEL(g_player_ped) > 0.3f
				|| ENTITY::IS_ENTITY_IN_AIR(g_player_ped)
				|| g_player_veh.isValid() // It still works in a vehicle, but it plays an animation. Hooked every animation native to try and prevent it (inc. scenarios) but nothing.
				)
			{
				click.setResponse(LOC("GENFAIL"));
				return;
			}

			*ScriptGlobal(GLOBAL_NANO_DRONE).as<int*>() |= (1 << 23) | (1 << 24);
			TASK::CLEAR_PED_TASKS(g_player_ped);
		}
	};
}