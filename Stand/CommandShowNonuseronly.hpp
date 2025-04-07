#pragma once

#include "CommandToggle.hpp"

#include "Gui.hpp"
#include "MenuGrid.hpp"

namespace Stand
{
	class CommandShowNonuseronly : public CommandToggle
	{
	public:
		explicit CommandShowNonuseronly(CommandList* const parent)
			: CommandToggle(parent, LOC("SHWOTHP"), { CMDNAME("shownonuseronly") }, NOLABEL, true)
		{
		}

		void onChange(Click& click) final
		{
			g_menu_grid.updateSetting([this]
			{
				g_gui.show_nonuseronly = m_on;
			});
		}
	};
}
