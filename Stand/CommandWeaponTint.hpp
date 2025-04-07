#pragma once

#include "CommandSlider.hpp"

#include "AbstractEntity.hpp"
#include "natives.hpp"

namespace Stand
{
	class CommandWeaponTint : public CommandSlider
	{
	public:
		explicit CommandWeaponTint(CommandList* const parent)
			: CommandSlider(parent, LOC("GUNTINT"), CMDNAMES("weapontint"), NOLABEL, 0, 7, 0, 1, CMDFLAGS_SLIDER & ~CMDFLAG_SUPPORTS_STATE_OPERATIONS)
		{
		}

		void onChange(Click& click, int prev_value) final
		{
			if (click.type != CLICK_BULK)
			{
				ensureScriptThread(click, [this]
				{
					WEAPON::SET_PED_WEAPON_TINT_INDEX(g_player_ped, WEAPON::GET_SELECTED_PED_WEAPON(g_player_ped), value);
				});
			}
		}

		void onTickInGameViewport() final
		{
			if (!job_queued)
			{
				Hash weapon = WEAPON::GET_SELECTED_PED_WEAPON(g_player_ped);
				setMaxValue(WEAPON::GET_WEAPON_TINT_COUNT(weapon) - 1);
				Click click(CLICK_BULK, TC_SCRIPT_NOYIELD, true);
				setValue(click, WEAPON::GET_PED_WEAPON_TINT_INDEX(g_player_ped, weapon));
			}
		}

		void onTickInWebViewport() final
		{
			return onTickInWebViewportImplRedirect();
		}
	};
}
