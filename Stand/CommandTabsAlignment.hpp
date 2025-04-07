#pragma once

#include "CommandListSelect.hpp"

#include "Gui.hpp"
#include "Renderer.hpp"

namespace Stand
{
	class CommandTabsAlignment : public CommandListSelect
	{
	public:
		explicit CommandTabsAlignment(CommandList* parent)
			: CommandListSelect(parent, LOC("TA"), { CMDNAME("tabsalignment") }, NOLABEL, {
				{ALIGN_TOP_LEFT, LOC("RPOS_L"), CMDNAMES("left")},
				{ALIGN_TOP_CENTRE, LOC("ALIGN_CC"), CMDNAMES("centre", "center")},
				{ALIGN_TOP_RIGHT, LOC("RPOS_R"), CMDNAMES("right")},
			}, ALIGN_TOP_LEFT)
		{
		}

		void onChange(Click& click, long long prev_value) final
		{
			if (g_gui.canUpdateThemeSetting())
			{
				g_renderer.tabs_alignment = (Alignment)value;
			}
		}
	};
}
