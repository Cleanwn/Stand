#pragma once

#include "CommandListSelect.hpp"

#include "MenuGrid.hpp"
#include "Renderer.hpp"

namespace Stand
{
	class CommandTabsPos : public CommandListSelect
	{
	private:
		CommandToggleNoCorrelation* const tabs;

	public:
		explicit CommandTabsPos(CommandList* const parent, CommandToggleNoCorrelation* tabs)
			: CommandListSelect(parent, LOC("POS"), { CMDNAME("tabsposition") }, NOLABEL, {
				{LEFT, LOC("RPOS_L"), CMDNAMES("left")},
				{RIGHT, LOC("RPOS_R"), CMDNAMES("right")},
				{TOP, LOC("RPOS_T"), CMDNAMES("top")},
				{BOTTOM, LOC("RPOS_B"), CMDNAMES("bottom")},
			}, LEFT), tabs(tabs)
		{
		}

		void onChange(Click& click, long long prev_value) final
		{
			if (tabs->m_on)
			{
				g_menu_grid.updateSetting([this]
				{
					g_renderer.tabs_pos = (Direction)value;
				});
			}
		}
	};
}
