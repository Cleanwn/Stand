#include "CommandMarkedPlayers.hpp"

#include "CommandHistoricPlayer.hpp"
#include "PlayerHistory.hpp"

namespace Stand
{
	CommandMarkedPlayers::CommandMarkedPlayers(CommandList* parent)
		: CommandListHistoricPlayers(parent, LOC("HISTWJOINR"))
	{
	}

	std::vector<CommandHistoricPlayer*> CommandMarkedPlayers::collectPlayers() const
	{
		std::vector<CommandHistoricPlayer*> res{};
		for (const auto& _cmd : PlayerHistory::player_history_command->children)
		{
			if (_cmd->type == COMMAND_LIST_HISTORICPLAYER)
			{
				auto cmd = _cmd->as<CommandHistoricPlayer>();
				if (cmd->player->join_reactions)
				{
					res.emplace_back(cmd);
				}
			}
		}
		return res;
	}
}
