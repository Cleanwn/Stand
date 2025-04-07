#pragma once

#include "CommandColour.hpp"

#include "Gui.hpp"
#include "Renderer.hpp"

namespace Stand
{
	class CommandBlurTextColour : public CommandColour
	{
	public:
		explicit CommandBlurTextColour(CommandList* const parent)
			: CommandColour(parent, LOC("CLRBT"), { CMDNAME("unfocusedtext") }, NOLABEL, DirectX::SimpleMath::Color(1.0f, 1.0f, 1.0f, 1.0f))
		{
		}

		void onChange(Click& click) final
		{
			if (g_gui.canUpdateThemeSetting())
			{
				g_renderer.bgTextColour = getRGBA();
			}
		}
	};
}
