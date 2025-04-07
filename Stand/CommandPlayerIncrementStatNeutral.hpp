#pragma once

#include "CommandPlayerAction.hpp"

#include "AbstractPlayer.hpp"
#include "pointers.hpp"

namespace Stand
{
	class CommandPlayerIncrementStatNeutral : public CommandPlayerAction
	{
	private:
		const int stat_hash;

	public:
		explicit CommandPlayerIncrementStatNeutral(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, int stat_hash)
			: CommandPlayerAction(parent, std::move(menu_name), std::move(command_names), NOLABEL, COMMANDPERM_NEUTRAL), stat_hash(stat_hash)
		{
		}

		void onClick(Click& click) final
		{
			if (click.inOnline())
			{
				DEF_P2;
				if (pp->single && pp->getPlayers(true).at(0) == g_player)
				{
					click.setResponse(LOC("CMDOTH"));
				}
				else
				{
					for (AbstractPlayer p : pp->getPlayers(false))
					{
						pointers::create_and_send_increment_stat_event(stat_hash, 16, p.getCNetGamePlayer());
					}
				}
			}
		}
	};
}
