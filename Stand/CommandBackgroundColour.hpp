#pragma once

#include "CommandColour.hpp"

#include "Gui.hpp"
#include "Renderer.hpp"

namespace Stand
{
	class CommandBackgroundColour : public CommandColour
	{
	public:
		explicit CommandBackgroundColour(CommandList* const parent)
			: CommandColour(parent, LOC("CLRBR"), { CMDNAME("background") }, NOLABEL, 0, 0, 0, 77)
		{
		}

		void onChange(Click& click) final
		{
			if (g_gui.canUpdateThemeSetting())
			{
				g_renderer.bgRectColour = getRGBA();
			}
		}
	};
}
