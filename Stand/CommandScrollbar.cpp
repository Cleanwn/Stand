#include "CommandScrollbar.hpp"

#include "MenuGrid.hpp"

namespace Stand
{
	CommandScrollbar::CommandScrollbar(CommandList* parent)
		: CommandListSelect(parent, LOC("SCRLBR"), {}, NOLABEL, {
			{(long long)ScrollbarMode::ENABLED, LOC("ON")},
			{(long long)ScrollbarMode::ENABLED_WHEN_NEEDED, LOC("SCRLBR_1")},
			{(long long)ScrollbarMode::DISABLED, LOC("DOFF")},
		}, (long long)ScrollbarMode::ENABLED_WHEN_NEEDED)
	{
	}

	void CommandScrollbar::onChange(Click& click, long long prev_value)
	{
		g_menu_grid.updateSetting([this]
		{
			g_menu_grid.scrollbar_mode = (ScrollbarMode)value;
		});
	}
}
