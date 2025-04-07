#include "CommandOutfitPropVariation.hpp"

#include "AbstractEntity.hpp"
#include "CommandLockOutfit.hpp"
#include "CommandOutfitProp.hpp"
#include "natives.hpp"

namespace Stand
{
	void CommandOutfitPropVariation::onChange(Click& click, int prev_value)
	{
		ensureScriptThread(click, [this]
		{
			PED::SET_PED_PROP_INDEX(g_player_ped, data->id, CommandLockOutfit::ped_prop_commands[data->id].first->value, value, true, 1);
		});
	}

	void CommandOutfitPropVariation::onDrawableChange()
	{
		Click click(CLICK_AUTO, TC_SCRIPT_NOYIELD);
		setValueIndicator(click, 0);
		updateRange();
	}

	void CommandOutfitPropVariation::updateRange()
	{
		setMaxValue(PED::GET_NUMBER_OF_PED_PROP_TEXTURE_VARIATIONS(g_player_ped, data->id, CommandLockOutfit::ped_prop_commands[data->id].first->value) - 1);
	}
}
