#pragma once

#include "CommandToggle.hpp"

#include "Gui.hpp"
#include "MenuGrid.hpp"

namespace Stand
{
	class CommandHelpText : public CommandToggle
	{
	public:
		explicit CommandHelpText(CommandList* const parent)
			: CommandToggle(parent, LOC("SHWHLPTXT"), CMDNAMES("showhelptext"), LOC("IPSUM"), true)
		{
		}

		void onChange(Click& click) final
		{
			g_menu_grid.updateSetting([this]
			{
				g_gui.show_help_text = m_on;
			});
		}
	};
}
