#pragma once

#include "CommandToggle.hpp"

#include "AllEntitiesEveryTick.hpp"

namespace Stand
{
	class CommandPlayerNeedsToAimAtUser : public CommandToggle
	{
	public:
		explicit CommandPlayerNeedsToAimAtUser(CommandList* const parent)
			: CommandToggle(parent, LOC("AIMME"), {}, LOC("AIMMEP"), true)
		{
		}

		void onChange(Click& click) final
		{
			AllEntitiesEveryTick::player_needs_to_aim_at_user = m_on;
		}
	};
}
