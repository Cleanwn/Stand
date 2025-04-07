#pragma once

#include "CommandToggle.hpp"

#include "get_current_time_millis.hpp"
#include "Gui.hpp"
#include "natives.hpp"
#include "TpUtil.hpp"

namespace Stand
{
	class CommandAutoTpWp : public CommandToggle
	{
	public:
		explicit CommandAutoTpWp(CommandList* const parent)
			: CommandToggle(parent, LOC("AUTOTPWP"), { CMDNAME("autotpwp") }, LOC("AUTOTPWP_H"))
		{
		}

		void onChange(Click& click) final
		{
			onChangeToggleScriptTickEventHandler(click, []()
			{
				static long long last_tp = 0;
				if (!g_gui.waypoint.isNull())
				{
					const time_t now = get_current_time_millis();
					if (last_tp + 1000 < now)
					{
						last_tp = now;
						Util::toast(LOC("AUTOTPWP_T"));
						HUD::DELETE_WAYPOINTS_FROM_THIS_PLAYER();
						TpUtil::teleportWithRedirects(g_gui.waypoint);
					}
				}
			});
		}
	};
}
