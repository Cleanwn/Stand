#pragma once

#include "CommandToggle.hpp"

#include <soup/time.hpp>

namespace Stand
{
	class CommandClockSys : public CommandToggle
	{
	public:
		explicit CommandClockSys(CommandList* const parent)
			: CommandToggle(parent, LOC("SYSTIME"), CMDNAMES_OBF("systime", "systemtime"))
		{
		}

		void onEnable(Click& click) final
		{
			// Radio behaviour with "Lock Time"
			{
				auto derived = click.deriveAuto();
				parent->as<CommandClock>()->lock_cmd->setStateBool(derived, false);
			}

			registerScriptTickEventHandler(click, [this]
			{
				HANDLER_CHECK(m_on);
				auto dt = soup::time::datetimeLocal(soup::time::unixSeconds());
				parent->as<CommandClock>()->locked_time.hour = dt.hour;
				parent->as<CommandClock>()->locked_time.minute = dt.minute;
				parent->as<CommandClock>()->locked_time.second = dt.second;
				NETWORK::NETWORK_OVERRIDE_CLOCK_TIME(parent->as<CommandClock>()->locked_time.hour, parent->as<CommandClock>()->locked_time.minute, parent->as<CommandClock>()->locked_time.second);
				HANDLER_END;
			});
		}
	};
}
