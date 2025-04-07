#pragma once

#include "CommandColourCustom.hpp"

#include "Gui.hpp"
#include "Renderer.hpp"

namespace Stand
{
	class CommandPrimaryColour : public CommandColourCustom
	{
	public:
		explicit CommandPrimaryColour(CommandList* const parent)
			: CommandColourCustom(parent, LOC("CLRFR"), { CMDNAME("primary") }, NOLABEL, DirectX::SimpleMath::Color(1.0f, 0.0f, 1.0f, 1.0f))
		{
		}

		void onChange(Click& click) final
		{
			if (g_gui.canUpdateThemeSetting())
			{
				g_renderer.focusRectColour = getRGBA();
			}
		}
	};
}
