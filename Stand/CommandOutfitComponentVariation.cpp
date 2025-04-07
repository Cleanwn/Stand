#include "CommandOutfitComponentVariation.hpp"

#include "AbstractEntity.hpp"
#include "CommandOutfitComponent.hpp"
#include "CommandLockOutfit.hpp"
#include "natives.hpp"

namespace Stand
{
	void CommandOutfitComponentVariation::onChange(Click& click, int prev_value)
	{
		ensureScriptThread(click, [this]
		{
			if (data->id == PV_COMP_HAIR)
			{
				PED::SET_PED_COMPONENT_VARIATION(g_player_ped, data->id, CommandLockOutfit::ped_component_commands[data->id].first->value, 0, 0);
				int highlight = CommandLockOutfit::hair_highlight->value;
				if (highlight == -1)
				{
					highlight = value;
				}
				PED::SET_PED_HAIR_TINT(g_player_ped, value, highlight);
			}
			else
			{
				PED::SET_PED_COMPONENT_VARIATION(g_player_ped, data->id, CommandLockOutfit::ped_component_commands[data->id].first->value, value, 0);
			}
		});
	}

	void CommandOutfitComponentVariation::onDrawableChange()
	{
		Click click(CLICK_AUTO, TC_SCRIPT_NOYIELD);
		setValueIndicator(click, 0);
		updateRange();
	}

	void CommandOutfitComponentVariation::updateRange()
	{
		setMaxValue(PED::GET_NUMBER_OF_PED_TEXTURE_VARIATIONS(g_player_ped, data->id, CommandLockOutfit::ped_component_commands[data->id].first->value) - 1);
	}
}
