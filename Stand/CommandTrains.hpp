#pragma once

#include "CommandToggle.hpp"

#include "natives.hpp"

namespace Stand
{
	class CommandTrains : public CommandToggle
	{
	public:
		explicit CommandTrains(CommandList* const parent)
			: CommandToggle(parent, LOC("TRAINS"), { CMDNAME("trains"), CMDNAME("iliketrains") }, LOC("TRAINS_H"))
		{
		}

		void onEnable(Click& click) final
		{
			ensureScriptThread(click, []
			{
				for (int i = 0; i <= 26; i++)
				{
					VEHICLE::SET_TRAIN_TRACK_SPAWN_FREQUENCY(i, 0);
				}
			});
		}

		void onDisable(Click& click) final
		{
			ensureScriptThread(click, []
			{
				for (int i = 0; i <= 26; i++)
				{
					VEHICLE::SET_TRAIN_TRACK_SPAWN_FREQUENCY(i, 120000);
				}
			});
		}
	};
}
