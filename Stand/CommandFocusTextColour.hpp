#pragma once

#include "CommandColour.hpp"

#include "Gui.hpp"
#include "Renderer.hpp"

namespace Stand
{
	class CommandFocusTextColour : public CommandColour
	{
	public:
		explicit CommandFocusTextColour(CommandList* const parent)
			: CommandColour(parent, LOC("CLRFT"), { CMDNAME("focustext") }, NOLABEL, DirectX::SimpleMath::Color(1.0f, 1.0f, 1.0f, 1.0f))
		{
		}

		void onChange(Click& click) final
		{
			if (g_gui.canUpdateThemeSetting())
			{
				g_renderer.focusTextColour = getRGBA();
			}
		}
	};
}
