#pragma once

#include "CommandToggle.hpp"

#include "Gui.hpp"
#include "Renderer.hpp"

namespace Stand
{
	class CommandTabs : public CommandToggle
	{
	public:
		CommandListSelect* pos;

		explicit CommandTabs(CommandList* const parent)
			: CommandToggle(parent, LOC("TABS"), CMDNAMES("tabs"), NOLABEL, true)
		{
		}

		void onChange(Click& click) final
		{
			if (g_gui.canUpdateThemeSetting())
			{
				// Keep focus when changing profile, but not when doing inital load.
				Command* command = (g_gui.unlock_recover_state == 2 ? g_gui.getCurrentMenuFocus() : nullptr);
				g_renderer.tabs_pos = m_on ? (Direction)pos->value : NONE;
				g_gui.updateTabs(click.thread_context);
				if (command != nullptr)
				{
					command->focus(click.thread_context);
				}
				g_menu_grid.update();
			}
		}
	};
}
