#pragma once

#include "CommandToggle.hpp"

#include "AbstractEntity.hpp"
#include "ePedConfigFlags.hpp"
#include "natives.hpp"

namespace Stand
{
	class CommandLeaveEngineRunning : public CommandToggle
	{
	public:
		explicit CommandLeaveEngineRunning(CommandList* const parent)
			: CommandToggle(parent, LOC("LVENGRUN"), { CMDNAME("leaveengineon") })
		{
		}

		void onChange(Click& click) final
		{
			onChangeToggleScriptTickEventHandler(click, []
			{
				g_player_ped.setConfigFlag(CPED_CONFIG_FLAG_LeaveEngineOnWhenExitingVehicles, true);
			});
			if (!m_on)
			{
				g_player_ped.setConfigFlag(CPED_CONFIG_FLAG_LeaveEngineOnWhenExitingVehicles, false);
			}
		}
	};
}