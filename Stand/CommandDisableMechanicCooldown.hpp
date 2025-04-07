#pragma once

#include "CommandToggle.hpp"

#include "is_session.hpp"
#include "ScriptGlobal.hpp"

namespace Stand
{
	class CommandDisableMechanicCooldown : public CommandToggle
	{
	public:
		explicit CommandDisableMechanicCooldown(CommandList* const parent)
			: CommandToggle(parent, LOC("NOMECHCDLN"), combineCommandNames(CMDNAMES("no", "block", "disable", "bypass"), CMDNAMES("mechaniccooldown", "mechanicooldown")))
		{
		}

		void onChange(Click& click) final
		{
			onChangeToggleScriptTickEventHandler(click, []
			{
				if (is_session_started())
				{
					*ScriptGlobal(GLOBAL_CONTACT_TIMEOUT).at(GLOBAL_CONTACT_TIMEOUT_MECHANIC, GLOBAL_CONTACT_TIMEOUT_SIZE).as<int*>() = 0;
				}
			});
		}
	};
}
