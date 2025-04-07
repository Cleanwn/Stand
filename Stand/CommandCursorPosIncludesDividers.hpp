#pragma once

#include "CommandToggle.hpp"

#include "Gui.hpp"

namespace Stand
{
	class CommandCursorPosIncludesDividers : public CommandToggle
	{
	public:
		explicit CommandCursorPosIncludesDividers(CommandList* const parent)
			: CommandToggle(parent, LOC("CRSPOSIDIV"))
		{
		}

		void onChange(Click& click) final
		{
			if (g_gui.canUpdateThemeSetting())
			{
				g_gui.cursor_pos_includes_dividers = m_on;
			}
		}
	};
}
