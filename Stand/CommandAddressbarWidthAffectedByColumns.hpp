#pragma once

#include "CommandToggle.hpp"

#include "MenuGrid.hpp"

namespace Stand
{
	class CommandAddressbarWidthAffectedByColumns : public CommandToggle
	{
	public:
		explicit CommandAddressbarWidthAffectedByColumns(CommandList* parent)
			: CommandToggle(parent, LOC("WDTAFFBCLMNS"))
		{
		}

		void onChange(Click& click) final
		{
			g_menu_grid.updateSetting([this]
			{
				g_menu_grid.addressbar_width_affected_by_columns = m_on;
			});
		}
	};
}
