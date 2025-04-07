#pragma once

#include "CommandToggle.hpp"

#include "MenuGrid.hpp"

namespace Stand
{
	class CommandAddressbarWidthAffectedByScrollbar : public CommandToggle
	{
	public:
		explicit CommandAddressbarWidthAffectedByScrollbar(CommandList* parent)
			: CommandToggle(parent, LOC("WDTAFFBSCRLBR"))
		{
		}

		void onChange(Click& click) final
		{
			g_menu_grid.updateSetting([this]
			{
				g_menu_grid.addressbar_width_affected_by_scrollbar = m_on;
			});
		}
	};
}
