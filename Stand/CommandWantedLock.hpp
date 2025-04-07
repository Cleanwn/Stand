#pragma once

#include "CommandToggle.hpp"

#include "AbstractPlayer.hpp"
#include "CommandSlider.hpp"
#include "natives.hpp"

namespace Stand
{
	class CommandWantedLock : public CommandToggle
	{
	private:
		CommandWanted* const wanted_slider;

	public:
		explicit CommandWantedLock(CommandList* const parent, CommandWanted* wanted_slider)
			: CommandToggle(parent, LOC("LKWNTD"), { CMDNAME("lockwantedlevel"), CMDNAME("wantedlock") }), wanted_slider(wanted_slider)
		{
		}

		void onChange(Click& click) final
		{
			if (m_on)
			{
				registerScriptTickEventHandler(click, [=]()
				{
					if (m_on)
					{
						if (g_player.getWantedLevel() != wanted_slider->value)
						{
							g_player.setWantedLevel(wanted_slider->value);
						}
						PLAYER::SET_MAX_WANTED_LEVEL(wanted_slider->value == 0 ? 0 : 5);
						return true;
					}
					if (wanted_slider->value == 0)
					{
						PLAYER::SET_MAX_WANTED_LEVEL(5);
					}
					return false;
				});
			}
		}
	};
}
