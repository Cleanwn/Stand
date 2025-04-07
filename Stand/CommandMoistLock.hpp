#pragma once

#include "CommandToggle.hpp"

#include "CommandSlider.hpp"
#include "natives.hpp"

namespace Stand
{
	class CommandMoistLock : public CommandToggle
	{
	public:
		CommandSlider* const moist;

		explicit CommandMoistLock(CommandList* const parent, CommandSlider* moist)
			: CommandToggle(parent, LOC("WETLCK"), { CMDNAME("lockwetness") }), moist(moist)
		{
		}

		void onEnable(Click& click) final
		{
			registerScriptTickEventHandler(click, [=]()
			{
				if (!m_on)
				{
					return false;
				}
				if (moist->value == 0)
				{
					PED::CLEAR_PED_WETNESS(g_player_ped);
				}
				else
				{
					PED::SET_PED_WETNESS_HEIGHT(g_player_ped, (((float)moist->value * 0.2f) - 110.0f) / 100.0f);
					PED::SET_PED_WETNESS_ENABLED_THIS_FRAME(g_player_ped);
				}
				return true;
			});
		}
	};
}
