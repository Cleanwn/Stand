#pragma once

#include "CommandSliderNamedValue.hpp"

#include "StatCache.hpp"

namespace Stand
{
	class CommandMaxHealth : public CommandSliderNamedValue<CommandSlider>
	{
	public:
		explicit CommandMaxHealth(CommandList* const parent)
			: CommandSliderNamedValue<CommandSlider>(parent, LOC("MXHLTH"), CMDNAMES("maxhealth", "maximumhealth"), NOLABEL, 200, LANG_GET_W("OVRDOFF"), 200, 15000, 200, 25)
		{
		}

		void onChange(Click& click, int prev_value) final
		{
			registerScriptTickEventHandler(click, [this, v{ value }]
			{
				HANDLER_CHECK(v == this->value);

				if (is_session_transition_active(true))
				{
					HANDLER_END;
				}

				if (value == min_value)
				{
					if (is_session_started())
					{
						g_player_ped.setMaxHealth((float)Util::getMaxHealthForRank(StatCache::rank));
						return false;
					}

					g_player_ped.setMaxHealth(200.0f);
					return false;
				}
				else if (!g_player_ped.isDead())
				{
					g_player_ped.setMaxHealth((float)value);
				}

				HANDLER_END;
			});
		}
	};
}