#pragma once

#include "CommandSlider.hpp"

namespace Stand
{
	class CommandCursorPadding : public CommandSlider
	{
	public:
		explicit CommandCursorPadding(CommandList* const parent)
			: CommandSlider(parent, LOC("SCRLGP"), { CMDNAME("cursorpadding") }, LOC("CRSPAD_H"), 0, 25, 2, 1)
		{
		}

		void onChange(Click& click, int prev_value) final
		{
			if (g_gui.canUpdateThemeSetting())
			{
				g_gui.cursor_padding = value;
				if (!click.isAuto())
				{
					this->focus(click.thread_context);
				}
			}
		}
	};
}
