#pragma once

#include "CommandToggle.hpp"

#include "AllEntitiesEveryTick.hpp"

namespace Stand
{
	class CommandNpcNeedsToAimAtUser : public CommandToggle
	{
	public:
		explicit CommandNpcNeedsToAimAtUser(CommandList* const parent)
			: CommandToggle(parent, LOC("AIMME"), {}, LOC("AIMMEN"), true)
		{
		}

		void onChange(Click& click) final
		{
			AllEntitiesEveryTick::npc_needs_to_aim_at_user = m_on;
		}
	};
}
