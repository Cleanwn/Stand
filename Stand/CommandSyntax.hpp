#pragma once

#include "CommandToggle.hpp"

#include "MenuGrid.hpp"
#include "Gui.hpp"

namespace Stand
{
	class CommandSyntax : public CommandToggle
	{
	public:
		explicit CommandSyntax(CommandList* const parent)
			: CommandToggle(parent, LOC("SHWSYNTX"), { CMDNAME("showsyntax") }, LOC("IPSUM"), true)
		{
		}

		void onChange(Click& click) final
		{
			g_menu_grid.updateSetting([this]
			{
				g_gui.show_syntax = m_on;
			});
		}
	};
}
