#pragma once

#include "CommandSlider.hpp"

#include "natives.hpp"

namespace Stand
{
	class CommandMoist : public CommandSlider
	{
	public:
		explicit CommandMoist(CommandList* const parent)
			: CommandSlider(parent, LOC("WET"), { CMDNAME("wetness"), CMDNAME("moisture") }, NOLABEL, 0, 1000, 0, 5, CMDFLAGS_SLIDER, COMMANDPERM_USERONLY, true)
		{
		}

		void onClick(Click& click) final
		{
			ensureScriptThread(click, [=]
			{
				if (value == 0)
				{
					PED::CLEAR_PED_WETNESS(g_player_ped);
				}
				else
				{
					PED::SET_PED_WETNESS_HEIGHT(g_player_ped, (((float)value * 0.2f) - 110.0f) / 100.0f);
				}
			});
		}
	};
}
