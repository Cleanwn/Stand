#pragma once

#include "CommandToggle.hpp"

#include "DrawUtil3d.hpp"
#include "Gui.hpp"
#include "Renderer.hpp"

namespace Stand
{
	class CommandArWaypoint : public CommandToggle
	{
	public:
		explicit CommandArWaypoint(CommandList* const parent)
			: CommandToggle(parent, LOC("ARWP"), { CMDNAME("arwaypoint"), CMDNAME("arwp") }, LOC("ARWP_H"))
		{
		}

		void onChange(Click& click) final
		{
			onChangeToggleScriptTickEventHandler(click, []()
			{
				if (!g_gui.waypoint.isNull()
					&& !DrawUtil3d::exclusive_ar
					)
				{
					DrawUtil3d::draw_ar_beacon(g_gui.waypoint);
				}
			});
		}
	};
}
