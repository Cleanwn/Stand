#pragma once

#include "CommandList.hpp"

#include "TimeOfDay.hpp"

namespace Stand
{
#pragma pack(push, 1)
	class CommandClock : public CommandList
	{
	public:
		CommandToggle* lock_cmd;
		CommandToggle* sys_cmd;

		bool smooth_transition = true;
		time_t last_sync = 0;
		TimeOfDay locked_time{};

		explicit CommandClock(CommandList* const parent);

		void setTime(TimeOfDay&& tod); // needs yieldable script thread if smooth_transition is true
	};
#pragma pack(pop)
}
