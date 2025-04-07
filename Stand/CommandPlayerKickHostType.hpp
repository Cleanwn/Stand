#pragma once

#include "CommandPlayerActionAggressive.hpp"

namespace Stand
{
	class CommandPlayerKickHostType : public CommandPlayerActionAggressive
	{
	public:
		const AbstractPlayer::eHostKickType type;

		explicit CommandPlayerKickHostType(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text, AbstractPlayer::eHostKickType type)
			: CommandPlayerActionAggressive(parent, std::move(menu_name), std::move(command_names), std::move(help_text)), type(type)
		{
		}

		void onClick(Click& click) final
		{
			if (click.isUltimateEdition() && click.isHost())
			{
				DEF_P2;
				std::vector<AbstractPlayer> players = pp->getPlayers(pp->single);
				if (pp->single && players.at(0) == g_player)
				{
					click.notAvailableOnUser();
				}
				else
				{
					for (const auto& p : players)
					{
						p.kickHost(type);
					}
				}
			}
		}
	};
}
