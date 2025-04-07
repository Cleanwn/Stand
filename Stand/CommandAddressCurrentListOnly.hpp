#pragma once

#include "CommandToggle.hpp"

namespace Stand
{
	class CommandAddressCurrentListOnly : public CommandToggle
	{
	public:
		explicit CommandAddressCurrentListOnly(CommandList* parent)
			: CommandToggle(parent, LOC("SHWCRLSTNLY"))
		{
		}

		void onChange(Click& click) final
		{
			g_menu_grid.updateSetting([this]
			{
				g_renderer.address_current_list_only = m_on;
			});
		}
	};
}
