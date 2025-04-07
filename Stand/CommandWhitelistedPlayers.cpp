#include "CommandWhitelistedPlayers.hpp"

#include "CommandHistoricPlayer.hpp"
#include "PlayerHistory.hpp"
#include "lang.hpp"

namespace Stand
{
	CommandWhitelistedPlayers::CommandWhitelistedPlayers(CommandList* parent)
		: CommandListHistoricPlayers(parent, LOC("HISTWWHT"))
	{
	}

	std::vector<CommandHistoricPlayer*> CommandWhitelistedPlayers::collectPlayers() const
	{
		std::vector<CommandHistoricPlayer*> res{};
		for (const auto& _cmd : PlayerHistory::player_history_command->children)
		{
			if (_cmd->type == COMMAND_LIST_HISTORICPLAYER)
			{
				auto cmd = _cmd->as<CommandHistoricPlayer>();
				if (cmd->player->flags & HP_WHITELIST)
				{
					res.emplace_back(cmd);
				}
			}
		}
		return res;
	}
}
