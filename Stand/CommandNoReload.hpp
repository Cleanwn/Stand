#pragma once

#include "CommandToggle.hpp"

#include "atStringHash.hpp"
#include "natives.hpp"

namespace Stand
{
	class CommandNoReload : public CommandToggle
	{
	public:
		explicit CommandNoReload(CommandList* const parent)
			: CommandToggle(parent, LOC("SKPRLD"), { CMDNAME("noreload"), CMDNAME("skipreloading") })
		{
		}

		void onChange(Click& click) final
		{
			onChangeToggleScriptTickEventHandler(click, []()
			{
				const Hash weapon_hash = WEAPON::GET_SELECTED_PED_WEAPON(g_player_ped);
				if (weapon_hash != ATSTRINGHASH("WEAPON_MINIGUN") && weapon_hash != ATSTRINGHASH("WEAPON_RAYMINIGUN"))
				{
					WEAPON::REFILL_AMMO_INSTANTLY(g_player_ped);
				}
			});
		}
	};
}
