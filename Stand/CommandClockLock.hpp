#pragma once

#include "CommandToggle.hpp"
#include "TimeOfDay.hpp"
#include "natives.hpp"

namespace Stand
{
	class CommandClockLock : public CommandToggle
	{
	public:
		explicit CommandClockLock(CommandList* const parent)
			: CommandToggle(parent, LOC("LKTIME"), { CMDNAME("locktime"), CMDNAME("pausetime") })
		{
		}

		void onEnable(Click& click) final
		{
			// Radio behaviour with "Use System Time"
			{
				auto derived = click.deriveAuto();
				parent->as<CommandClock>()->sys_cmd->setStateBool(derived, false);
			}

			parent->as<CommandClock>()->locked_time.hour = CLOCK::GET_CLOCK_HOURS();
			parent->as<CommandClock>()->locked_time.minute = CLOCK::GET_CLOCK_MINUTES();
			parent->as<CommandClock>()->locked_time.second = CLOCK::GET_CLOCK_SECONDS();
			registerScriptTickEventHandler(click, [this]
			{
				HANDLER_CHECK(m_on);
				if (parent->as<CommandClock>()->locked_time.isValid())
				{
					NETWORK::NETWORK_OVERRIDE_CLOCK_TIME(parent->as<CommandClock>()->locked_time.hour, parent->as<CommandClock>()->locked_time.minute, parent->as<CommandClock>()->locked_time.second);
				}
				HANDLER_END;
			});
		}

		[[nodiscard]] std::string getState() const final
		{
			auto state = CommandToggle::getState();
			if (m_on)
			{
				state.append(", ");
				state.append(parent->as<CommandClock>()->locked_time.toString());
			}
			return state;
		}

		void setState(Click& click, const std::string& _state) final
		{
			auto arr = soup::string::explode(_state, ", ");
			CommandToggle::setState(click, arr.at(0));
			if (m_on && arr.size() > 1)
			{
				parent->as<CommandClock>()->locked_time = TimeOfDay::fromString(arr.at(1));
			}
		}
	};
}
