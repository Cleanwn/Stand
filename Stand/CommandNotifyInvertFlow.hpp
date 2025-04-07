#pragma once

#include "CommandToggle.hpp"

#include "NotifyGrid.hpp"

namespace Stand
{
	class CommandNotifyInvertFlow : public CommandToggle
	{
	public:
		explicit CommandNotifyInvertFlow(CommandList* const parent)
			: CommandToggle(parent, LOC("INVFLOW"), {}, LOC("CUSTPOS_H"))
		{
		}

		void onChange(Click& click) final
		{
			if (m_on && g_notify_grid.next_to_map)
			{
				click.setResponse(LOC("CUSTPOS_H"));
			}
			g_notify_grid.updateSetting([on{ m_on }]
			{
				g_notify_grid.invert_flow = on;
			});
		}
	};
}
