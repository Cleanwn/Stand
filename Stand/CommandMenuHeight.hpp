#pragma once

#include "CommandSlider.hpp"

#include "Gui.hpp"
#include "MenuGrid.hpp"

namespace Stand
{
	class CommandMenuHeight : public CommandSlider
	{
	public:
		explicit CommandMenuHeight(CommandList* const parent)
			: CommandSlider(parent, LOC("MNUHGT"), { CMDNAME("menuheight") }, NOLABEL, 3, 255, 11, 1)
		{
		}

		void onChange(Click& click, int prev_value) final
		{
			g_menu_grid.updateSetting([this, thread_context{click.thread_context}]
			{
				g_gui.command_rows = value;
				g_gui.getCurrentMenuFocus()->focusInParent(thread_context);
			});
		}
	};
}
