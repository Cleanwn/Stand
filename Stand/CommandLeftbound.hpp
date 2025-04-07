#pragma once

#include "CommandListSelect.hpp"

namespace Stand
{
	class CommandLeftbound : public CommandListSelect
	{
	public:
		explicit CommandLeftbound(CommandList* const parent)
			: CommandListSelect(parent, LOC("LFTBND"), CMDNAMES("lefttextures", "leftextures", "leftsprites", "leftboundtextures", "leftboundsprites"), NOLABEL, {
				{ 0, LOC("DOFF")},
				{ (1 << 0) | (1 << 1) | (1 << 2), LOC("LFTBND_2")},
				{ (1 << 1) | (1 << 2), LOC("LFTBND_2C")},
				{ (1 << 0) | (1 << 1), LOC("LFTBND_1")},
				{ (1 << 1), LOC("LFTBND_1C")},
			}, 0)
		{
		}

		void onChange(Click& click, long long prev_value) final
		{
			g_menu_grid.updateSetting([value{value}]
			{
				g_menu_grid.left_space_before_all_commands = value & (1 << 0);
				g_menu_grid.leftbound_textures_toggles = value & (1 << 1);
				g_menu_grid.leftbound_textures_nontoggles = value & (1 << 2);
			});
		}

		void setState(Click& click, const std::string& state) final
		{
			if (state == "On")
			{
				return CommandListSelect::setState(click, "All Commands");
			}
			return CommandListSelect::setState(click, state);
		}
	};
}
