#pragma once

#include "CommandListAction.hpp"

#include "CommandListActionItem.hpp"
#include "natives.hpp"
#include "WorldState.hpp"

namespace Stand
{
	class CommandWorldState : public CommandListAction
	{
	public:
		explicit CommandWorldState(CommandList* const parent)
			: CommandListAction(parent, LOC("SWRLDST"), { CMDNAME("worldstate") }, LOC("SWRLDST_H"), {
				{0, LOC("SP"), CMDNAMES("storymode", "singleplayer", "sp")},
				{1, LOC("Online"), CMDNAMES("online", "mp")},
				{2, LOC("CAYO"), CMDNAMES("island", "cayoperico")},
			})
		{
		}

	private:
		static void islandToggle(bool t)
		{
			STREAMING::LOAD_GLOBAL_WATER_FILE(t ? 1 : 0);

			STREAMING::SET_ISLAND_ENABLED("HeistIsland", t);

			TASK::SET_SCENARIO_GROUP_ENABLED("Heist_Island_Peds", t);

			HUD::SET_USE_ISLAND_MAP(t);

			//invoke<Void>(0xF74B1FFA4A15FBEA, t); // no clue what it does

			AUDIO::SET_AUDIO_FLAG("PlayerOnDLCHeist4Island", t);
			AUDIO::SET_AMBIENT_ZONE_LIST_STATE_PERSISTENT("AZL_DLC_Hei4_Island_Zones", t, true);
			AUDIO::SET_AMBIENT_ZONE_LIST_STATE_PERSISTENT("AZL_DLC_Hei4_Island_Disabled_Zones", !t, true);
			if (!t)
			{
				AUDIO::RELEASE_NAMED_SCRIPT_AUDIO_BANK("DLC_HEI4/DLCHEI4_GENERIC_01");
			}

			//NETWORK::_0xC505036A35AFD01B(t); // no clue what it does
		}

	public:
		void onItemClick(Click& click, CommandListActionItem* item) final
		{
			CommandListAction::onItemClick(click, item);
			ensureScriptThread(click, [item]
			{
				if (item->value != 2)
				{
					islandToggle(false);
				}
				if (item->value == 0)
				{
					WorldState::setOnline(FALSE);
				}
				else
				{
					WorldState::setOnline(TRUE);
					if (item->value == 2)
					{
						islandToggle(true);
					}
				}
			});
		}
	};
}
