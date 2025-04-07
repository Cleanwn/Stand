#pragma once

#include "CommandToggle.hpp"

#include "natives.hpp"

namespace Stand
{
	class CommandAutoHeal : public CommandToggle
	{
	public:
		explicit CommandAutoHeal(CommandList* const parent)
			: CommandToggle(parent, LOC("DEMIGOD"), { CMDNAME("demigodmode"), CMDNAME("semigodmode") }, LOC("DEMIGOD_H"))
		{
		}

		void onChange(Click& script) final
		{
			onChangeToggleScriptTickEventHandler(script, []()
			{
				const auto max_health = (float)ENTITY::GET_ENTITY_MAX_HEALTH(g_player_ped);
				const float health = g_player_ped.getHealth();
				if (health > 0.0f && health < max_health)
				{
					g_player_ped.setHealth(max_health);
				}
			//});
			//toggleAsyncTickEventHandler([]
			//{
				const float max_armor = g_player_ped.getMaxArmour();
				if (g_player_ped.getArmour() != max_armor)
				{
					g_player_ped.setArmour(max_armor);
				}
			});
		}
	};
}
