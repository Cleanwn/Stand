#pragma once

#include "CommandToggle.hpp"

#include "is_session.hpp"

namespace Stand
{
	class CommandTrnsSpdupSpawn : public CommandToggle
	{
	public:
		explicit CommandTrnsSpdupSpawn(CommandList* const parent)
			: CommandToggle(parent, LOC("TRNS_SPDUP_SPWN"), CMDNAMES("speedupspawn"))
		{
		}

		void onChange(Click& click) final
		{
			onChangeToggleScriptTickEventHandler(click, []
			{
				if (is_session_transition_active(false))
				{
					*ScriptGlobal(g_SpawnData_bDontAskPermission).as<BOOL*>() = TRUE;
				}
			});
		}
	};
}
