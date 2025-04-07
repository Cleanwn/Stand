#pragma once

#include "CommandPosition2d.hpp"

#include "NotifyGrid.hpp"

namespace Stand
{
	class CommandNotifyPosition : public CommandPosition2d
	{
	public:
		explicit CommandNotifyPosition(CommandList* parent)
			: CommandPosition2d(parent, LOC("CUSTPOS"), CMDNAME("notify"), { 32, 532 }, LOC("CUSTPOS_H"))
		{
		}

		void onClick(Click& click) final
		{
			if (g_notify_grid.next_to_map
				|| g_toaster != &g_grid_toaster
				)
			{
				click.setResponse(LOC("CUSTPOS_H"));
			}
			return CommandPosition2d::onClick(click);
		}

		void onChange(Position2d&& pos) const final
		{
			if (!g_notify_grid.next_to_map)
			{
				g_notify_grid.origin = std::move(pos);
				g_notify_grid.updatePositions();
			}
		}
	};
}
