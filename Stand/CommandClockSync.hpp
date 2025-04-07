#pragma once

#include "CommandAction.hpp"

namespace Stand
{
	class CommandClockSync : public CommandAction
	{
	public:
		explicit CommandClockSync(CommandList* parent)
			: CommandAction(parent, LOC("CLKSYNC"), CMDNAMES("syncclock", "synclock", "clocksync", "synctime", "timesync"))
		{
		}

		void onClick(Click& click) final
		{
			if (click.inOnline())
			{
				if (parent->as<CommandClock>()->smooth_transition
					&& !parent->as<CommandClock>()->locked_time.isValid()
					)
				{
					parent->as<CommandClock>()->last_sync = get_current_time_millis();
					NETWORK::NETWORK_CLEAR_CLOCK_TIME_OVERRIDE();
				}
				else
				{
					ensureYieldableScriptThread(click, [this]
					{
						TimeOfDay tod;
						NETWORK::NETWORK_GET_GLOBAL_MULTIPLAYER_CLOCK(&tod.hour, &tod.minute, &tod.second);
						parent->as<CommandClock>()->setTime(std::move(tod));
					});
				}
			}
		}
	};
}
