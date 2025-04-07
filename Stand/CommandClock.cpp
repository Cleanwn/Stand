#include "CommandClock.hpp"

#include "CommandDivider.hpp"
#include "CommandTogglePointer.hpp"
#include "get_current_time_millis.hpp"
#include "is_session.hpp"
#include "natives.hpp"
#include "Script.hpp"
#include "timescale.hpp"

#include "CommandClockSync.hpp"
#include "CommandClockTime.hpp"
#include "CommandClockLock.hpp"
#include "CommandClockSys.hpp"
#include "CommandClockSpeed.hpp"
#include "CommandClockPreset.hpp"

namespace Stand
{
	CommandClock::CommandClock(CommandList* const parent)
		: CommandList(parent, LOC("CLK"), {}, LOC("GEN_H_LOC"))
	{
		this->createChild<CommandClockSync>();
		this->createChild<CommandClockTime>();
		lock_cmd = this->createChild<CommandClockLock>();
		sys_cmd = this->createChild<CommandClockSys>();
		this->createChild<CommandClockSpeed>();
		this->createChild<CommandTogglePointer>(&smooth_transition, LOC("SMTHTRNS"), CMDNAMES("timesmoothing"));

		this->createChild<CommandDivider>(LOC("PRSTS"));

		this->createChild<CommandClockPreset>(LOC("CLKPRST_12"), 12);
		this->createChild<CommandClockPreset>(LOC("CLKPRST_6"), 6);
		this->createChild<CommandClockPreset>(LOC("CLKPRST_0"), 0);
		this->createChild<CommandClockPreset>(LOC("CLKPRST_18"), 18);
	}

	void CommandClock::setTime(TimeOfDay&& tod)
	{
		if (sys_cmd->m_on)
		{
			return;
		}
		if (smooth_transition
			&& g_timescale >= 0.5f
			)
		{
			locked_time.invalidate();
			const auto target_secs = tod.asSeconds();
			while (true)
			{
				last_sync = get_current_time_millis();
				NETWORK::NETWORK_OVERRIDE_CLOCK_RATE(int(floorf(5.0f / g_timescale)));
				auto secs = TimeOfDay::current().asSeconds();
				if ((secs > target_secs && secs - target_secs < 600) || g_timescale < 0.5f)
				{
					break;
				}
				Script::current()->yield();
			}
			NETWORK::NETWORK_OVERRIDE_CLOCK_RATE(2000);
		}
		else if (is_session_started())
		{
			NETWORK::NETWORK_OVERRIDE_CLOCK_TIME(tod.hour, tod.minute, tod.second);
		}
		if (!is_session_started())
		{
			CLOCK::SET_CLOCK_TIME(tod.hour, tod.minute, tod.second);
		}
		last_sync = 0;
		locked_time = std::move(tod);
	}
}
