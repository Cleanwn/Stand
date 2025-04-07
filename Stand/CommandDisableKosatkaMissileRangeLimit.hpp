#pragma once

#include "CommandToggle.hpp"

#include "ScriptGlobal.hpp"

namespace Stand
{
	class CommandDisableKosatkaMissileRangeLimit : public CommandToggle
	{
	public:
		explicit CommandDisableKosatkaMissileRangeLimit(CommandList* const parent)
			: CommandToggle(parent, LOC("NOSUBMSLMAXRNGE"), CMDNAMES_OBF("no", "block", "disable", "bypass"), LOC("NOSUBMSLMAXRNGE_H"))
		{
			addSuffixToCommandNames(CMDNAME_OBF("kosatkamissilerangelimit"));
		}

		void onChange(Click& click) final
		{
			onChangeToggleScriptTickEventHandler(click, []
			{
				ScriptGlobal(GLOBAL_KOSATKA_MISSILE_MAX_DISTANCE).set(999999);
			});

			if (!m_on)
			{
				ensureScriptThread(click, []
				{
					ScriptGlobal(GLOBAL_KOSATKA_MISSILE_MAX_DISTANCE).set(4000);
				});
			}
		}
	};
}
