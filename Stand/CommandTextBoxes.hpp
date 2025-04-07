#pragma once

#include "CommandToggle.hpp"

namespace Stand
{
	class CommandTextBoxes : public CommandToggle
	{
	public:
		explicit CommandTextBoxes(CommandList* const parent)
			: CommandToggle(parent, LOC("TXTBOX"), { CMDNAME("showtextboundingboxes") })
		{
		}

		void onChange(Click& click) final
		{
			if (g_gui.canUpdateThemeSetting())
			{
				g_renderer.drawTextDebugRects = m_on;
			}
		}
	};
}
