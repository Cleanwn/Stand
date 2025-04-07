#pragma once

#include "CommandToggle.hpp"

#include "ScriptGlobal.hpp"

namespace Stand
{
	class CommandDisableNanoDroneRangeLimit : public CommandToggle
	{
	public:
		explicit CommandDisableNanoDroneRangeLimit(CommandList* const parent)
			: CommandToggle(parent, LOC("NONANOMAXRNGE"), CMDNAMES_OBF("no", "block", "disable", "bypass"))
		{
			addSuffixToCommandNames(CMDNAME_OBF("nanodronerangelimit"));
		}

		void onChange(Click& click) final
		{
			onChangeToggleScriptTickEventHandler(click, []
			{
				if (is_session_started())
				{
					ScriptGlobal(GLOBAL_NANO_DRONE).at(GLOBAL_NANO_DRONE_RANGE_LIMIT).set(99999.0f);
					ScriptGlobal(GLOBAL_TUNABLE_BASE).at(GLOBAL_TUNABLE_BB_TERRORBYTE_DRONE_HEIGHT_LIMIT).set(99999);
				}
			});

			if (is_session_started() && !m_on)
			{
				ensureScriptThread(click, []
				{
					ScriptGlobal(GLOBAL_NANO_DRONE).at(GLOBAL_NANO_DRONE_RANGE_LIMIT).set(0.0f); // It's an override. It will refer back to the tunable after this.
					ScriptGlobal(GLOBAL_TUNABLE_BASE).at(GLOBAL_TUNABLE_BB_TERRORBYTE_DRONE_HEIGHT_LIMIT).set(200);
				});
			}
		}
	};
}