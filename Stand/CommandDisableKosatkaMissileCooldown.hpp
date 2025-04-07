#pragma once

#include "CommandToggle.hpp"

#include "ScriptGlobal.hpp"

namespace Stand
{
	class CommandDisableKosatkaMissileCooldown : public CommandToggle
	{
	public:
		explicit CommandDisableKosatkaMissileCooldown(CommandList* const parent)
			: CommandToggle(parent, LOC("NOSUBMSLCLDN"), CMDNAMES_OBF("no", "block", "disable", "bypass"))
		{
			addSuffixToCommandNames(CMDNAME_OBF("kosatkamissilecooldown"));
		}

		void onChange(Click& click) final
		{
			onChangeToggleScriptTickEventHandler(click, []
			{
				ScriptGlobal(GLOBAL_KOSATKA_MISSILE_COOLDOWN).set(0);
			});

			if (!m_on)
			{
				ensureScriptThread(click, []
				{
					ScriptGlobal(GLOBAL_KOSATKA_MISSILE_COOLDOWN).set(60000);
				});
			}
		}
	};
}
