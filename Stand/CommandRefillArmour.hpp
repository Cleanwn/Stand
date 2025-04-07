#pragma once

#include "CommandAction.hpp"

#include "AbstractEntity.hpp"

namespace Stand
{
	class CommandRefillArmour : public CommandAction
	{
	public:
		explicit CommandRefillArmour(CommandList* const parent)
			: CommandAction(parent, LOC("RFARM"), { CMDNAME("refillarmour") })
		{
		}

		void onClick(Click& click) final
		{
			g_player_ped.setArmour(g_player_ped.getMaxArmour());
		}
	};
}
