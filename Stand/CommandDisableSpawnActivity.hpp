#pragma once

#include "CommandToggle.hpp"

#include "is_session.hpp"

namespace Stand
{
	class CommandDisableSpawnActivity : public CommandToggle
	{
	public:
		explicit CommandDisableSpawnActivity(CommandList* const parent)
			: CommandToggle(parent, LOC("NSPWNACTY"), CMDNAMES("nospawnactivities", "nospawnactivity"), LOC("NSPWNACTY_H"))
		{
		}

		void onChange(Click& click) final
		{
			onChangeToggleScriptTickEventHandler(click, []
			{
				if (is_session_transition_active(false))
				{
					*ScriptGlobal(g_TransitionSpawnData_bIgnoreSpawnActivity).as<BOOL*>() = TRUE;
				}
			});
		}
	};
}
