#include "CommandOutfitProp.hpp"

#include "AbstractEntity.hpp"
#include "CommandLockOutfit.hpp"
#include "CommandOutfitPropVariation.hpp"
#include "natives.hpp"

namespace Stand
{
	void CommandOutfitProp::onChange(Click& click, int prev_value)
	{
		ensureScriptThread(click, [this]
		{
			if (value == -1)
			{
				PED::CLEAR_PED_PROP(g_player_ped, data->id, 1);
			}
			else
			{
				PED::SET_PED_PROP_INDEX(g_player_ped, data->id, value, 0, true, 1);
			}
			CommandLockOutfit::ped_prop_commands[data->id].second->onDrawableChange();
		});
	}
}
