#pragma once

#include "CommandToggle.hpp"

#include "Gui.hpp"
#include "MenuGrid.hpp"

namespace Stand
{
	class CommandShowSliderBehaviour : public CommandToggle
	{
	public:
		explicit CommandShowSliderBehaviour(CommandList* const parent)
			: CommandToggle(parent, LOC("SHWSLDBHV"), { CMDNAME("showsliderbehaviour") }, NOLABEL, true)
		{
		}

		void onChange(Click& click) final
		{
			g_menu_grid.updateSetting([this]
			{
				g_gui.show_slider_behaviour = m_on;
			});
		}
	};
}
