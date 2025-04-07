#pragma once

#include "CommandToggle.hpp"

#include "Script.hpp"
#include "ScriptGlobal.hpp"

namespace Stand
{
	class CommandNoWorkCooldown : public CommandToggle
	{
	public:
		explicit CommandNoWorkCooldown(CommandList* const parent)
			: CommandToggle(parent, LOC("NWRKCLD"), combineCommandNames(CMDNAMES("no", "block", "disable", "bypass"), std::vector<CommandName>{ CMDNAME("workcooldown"), CMDNAME("vipworkcooldown") }))
		{
		}

		void onChange(Click& click) final
		{
			onChangeToggleScriptTickEventHandler(click, []
			{
				if (*pointers::is_session_started)
				{
					for (uint32_t i = 0; i <= 45; ++i)
					{
						*ScriptGlobal(GLOBAL_VIP_COOLDOWNS + i + 1).as<BOOL*>() = true;
					}

					ScriptGlobal(GLOBAL_VEHICLE_EXPORT_COOLDOWN).set<int>(0);
				}
			});
		}
	};
}
