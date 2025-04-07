#pragma once

#include "CommandToggle.hpp"

#include "Gui.hpp"
#include "MenuGrid.hpp"

namespace Stand
{
	class CommandShowCmdOwners : public CommandToggle
	{
	public:
		explicit CommandShowCmdOwners(CommandList* const parent)
			: CommandToggle(parent, LOC("SHWCMDOWNR"), { CMDNAME("showowners") }, NOLABEL, true)
		{
		}

		void onChange(Click& click) final
		{
			g_menu_grid.updateSetting([this]
			{
				g_gui.show_cmd_owner = m_on;
			});
		}
	};
}
