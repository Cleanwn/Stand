#include "CommandStarredPlayers.hpp"

#include "CommandHistoricPlayer.hpp"
#include "PlayerHistory.hpp"

namespace Stand
{
	CommandStarredPlayers::CommandStarredPlayers(CommandList* parent)
		: CommandListHistoricPlayers(parent, LOC("PLYNOTED"))
	{
	}

	std::vector<CommandHistoricPlayer*> CommandStarredPlayers::collectPlayers() const
	{
		std::vector<CommandHistoricPlayer*> res{};
		for (const auto& _cmd : PlayerHistory::player_history_command->children)
		{
			if (_cmd->type == COMMAND_LIST_HISTORICPLAYER)
			{
				auto cmd = _cmd->as<CommandHistoricPlayer>();
				if (!cmd->player->note.empty())
				{
					res.emplace_back(cmd);
				}
			}
		}
		return res;
	}
}
