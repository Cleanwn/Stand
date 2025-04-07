#pragma once

#include "CommandToggle.hpp"

#include "MenuGrid.hpp"
#include "Renderer.hpp"

namespace Stand
{
	class CommandShowAddressbar : public CommandToggle
	{
	public:
		explicit CommandShowAddressbar(CommandList* const parent)
			: CommandToggle(parent, LOC("ADDRBAR"), { CMDNAME("addressbar") }, NOLABEL, true)
		{
		}

		void onChange(Click& click) final
		{
			g_menu_grid.updateSetting([this]
			{
				g_renderer.show_addressbar = m_on;
			});
		}
	};
}
