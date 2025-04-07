#pragma once

#include "CommandSliderProximity.hpp"

#include "AllEntitiesEveryTick.hpp"

namespace Stand
{
	class CommandPunishableProximity : public CommandSliderProximity
	{
	public:
		explicit CommandPunishableProximity(CommandList* const parent)
			: CommandSliderProximity(parent, { CMDNAME("punishableproximity") })
		{
		}

		void onChange(Click& click, int prev_value) final
		{
			AllEntitiesEveryTick::npc_punishable_proximity = getFloatValue();
		}
	};
}
