#pragma once

#include "CommandPosition2d.hpp"

#include "MenuGrid.hpp"

namespace Stand
{
	class CommandMenuPosition : public CommandPosition2d
	{
	public:
		explicit CommandMenuPosition(CommandList* parent)
			: CommandPosition2d(parent, LOC("MNUPOS"), CMDNAME("menu"), MenuGrid::default_origin)
		{
		}

		void onChange(Position2d&& pos) const final
		{
			g_menu_grid.updateSettingSimple([pos{ std::move(pos) }]() mutable
			{
				g_menu_grid.origin = std::move(pos);
			});
		}
	};
}
