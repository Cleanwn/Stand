#pragma once

#include "CommandToggle.hpp"

#include "natives.hpp"

namespace Stand
{
	class CommandDisablePeds : public CommandToggle
	{
	public:
		explicit CommandDisablePeds(CommandList* const parent)
			: CommandToggle(parent, LOC("DOFF2"), { CMDNAME("nopedestrians"), CMDNAME("nopeds") })
		{
		}

		Label getActivationName() const final
		{
			return LOC("DPEDS_A");
		}

		void onEnable(Click& click) final
		{
			registerScriptTickEventHandler(click, [=]()
			{
				if (m_on)
				{
					STREAMING::SET_REDUCE_PED_MODEL_BUDGET(true);
					STREAMING::SET_PED_POPULATION_BUDGET(0);
					return true;
				}
				else
				{
					STREAMING::SET_REDUCE_PED_MODEL_BUDGET(false);
					STREAMING::SET_PED_POPULATION_BUDGET(3);
					MISC::POPULATE_NOW();
					return false;
				}
			});
		}
	};
}
