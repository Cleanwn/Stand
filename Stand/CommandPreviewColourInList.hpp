#pragma once

#include "CommandToggle.hpp"

#include "Gui.hpp"
#include "Renderer.hpp"

namespace Stand
{
	class CommandPreviewColourInList : public CommandToggle
	{
	public:
		explicit CommandPreviewColourInList(CommandList* const parent)
			: CommandToggle(parent, LOC("PRCWCLRLST"), {}, NOLABEL, true)
		{
		}

		void onChange(Click& click) final
		{
			if (g_gui.canUpdateThemeSetting())
			{
				g_renderer.preview_colour_in_list = m_on;
			}
		}
	};
}
