#pragma once

#include "CommandToggle.hpp"

#include "ScriptGlobal.hpp"

namespace Stand
{
	class CommandDisablePassiveCooldown : public CommandToggle
	{
	public:
		explicit CommandDisablePassiveCooldown(CommandList* const parent)
			: CommandToggle(parent, LOC("NOPSSVCLDN"), CMDNAMES("no", "block", "disable", "bypass"))
		{
			addSuffixToCommandNames(CMDNAME("passivecooldown"));
		}

		void onChange(Click& click) final
		{
			onChangeToggleScriptTickEventHandler(click, []
			{
				*ScriptGlobal(GLOBAL_PASSIVE_COOLDOWN).as<int*>() = 0;
				*ScriptGlobal(GLOBAL_PASSIVE_KILL_COOLDOWN).as<int*>() = 0;
				*ScriptGlobal(GLOBAL_PASSIVE_EXIT_COOLDOWN).as<int*>() = 0;
			});
			if (!m_on)
			{
				ensureScriptThread(click, []
				{
					*ScriptGlobal(GLOBAL_PASSIVE_EXIT_COOLDOWN).as<int*>() = 30000;
				});
			}
		}
	};
}
