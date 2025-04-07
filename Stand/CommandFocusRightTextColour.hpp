#pragma once

#include "CommandColour.hpp"

#include "Gui.hpp"
#include "Renderer.hpp"

namespace Stand
{
	class CommandFocusRightTextColour : public CommandColour
	{
	public:
		explicit CommandFocusRightTextColour(CommandList* const parent, CommandColour* with_copy_from)
			: CommandColour(parent, LOC("CLRFRT"), { CMDNAME("focusrighttext") }, NOLABEL, DirectX::SimpleMath::Color(1.0f, 1.0f, 1.0f, 1.0f), true, CMDFLAGS_COLOUR, with_copy_from)
		{
		}

		void onChange(Click& click) final
		{
			if (g_gui.canUpdateThemeSetting())
			{
				g_renderer.focusRightTextColour = getRGBA();
			}
		}
	};
}
