#pragma once

#include "CommandPosition2d.hpp"

#include "drawDebugText.hpp"

namespace Stand
{
	class CommandInfoPos : public CommandPosition2d
	{
	private:
		bool was_active = false;

	public:
		explicit CommandInfoPos(CommandList* parent)
			: CommandPosition2d(parent, LOC("POS"), CMDNAME("infotext"), { 1886, 98 })
		{
		}

		void onActiveListUpdate() final
		{
			if (isCurrentUiOrWebList())
			{
				g_force_debug_text = true;
				was_active = true;
			}
			else if (was_active)
			{
				g_force_debug_text = false;
				was_active = false;
			}
		}

		void onChange(Position2d&& pos) const final
		{
			if (g_gui.canUpdateThemeSetting())
			{
				g_renderer.info_x = float(pos.x);
				g_renderer.info_y = float(pos.y);
			}
		}
	};
}
