#pragma once

#include "CommandColour.hpp"

#include "Gui.hpp"
#include "Renderer.hpp"

namespace Stand
{
	class CommandBlurRightTextColour : public CommandColour
	{
	public:
		explicit CommandBlurRightTextColour(CommandList* const parent, CommandColour* with_copy_from)
			: CommandColour(parent, LOC("CLRBRT"), { CMDNAME("unfocusedrighttext") }, NOLABEL, DirectX::SimpleMath::Color(1.0f, 1.0f, 1.0f, 1.0f), true, CMDFLAGS_COLOUR, with_copy_from)
		{
		}

		void onChange(Click& click) final
		{
			if (g_gui.canUpdateThemeSetting())
			{
				g_renderer.bgRightTextColour = getRGBA();
			}
		}
	};
}
