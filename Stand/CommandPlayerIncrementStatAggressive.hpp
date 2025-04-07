#pragma once

#include "CommandPlayerActionAggressive.hpp"

#include "AbstractPlayer.hpp"
#include "pointers.hpp"

namespace Stand
{
	class CommandPlayerIncrementStatAggressive : public CommandPlayerActionAggressive
	{
	private:
		const int stat_hash;

	public:
		explicit CommandPlayerIncrementStatAggressive(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, int stat_hash)
			: CommandPlayerActionAggressive(parent, std::move(menu_name), std::move(command_names)), stat_hash(stat_hash)
		{
		}

		void onClick(Click& click) final
		{
			if (click.inOnline())
			{
				for (AbstractPlayer p : PP_PTR->getPlayers())
				{
					if (p == g_player)
					{
						click.setResponse(LOC("CMDOTH"));
					}
					else
					{
						pointers::create_and_send_increment_stat_event(stat_hash, 16, p.getCNetGamePlayer());
					}
				}
			}
		}
	};
}
