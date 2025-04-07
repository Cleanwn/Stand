#include "CommandTrackedPlayers.hpp"

#include "CommandHistoricPlayer.hpp"
#include "PlayerHistory.hpp"

namespace Stand
{
	CommandTrackedPlayers::CommandTrackedPlayers(CommandList* parent)
		: CommandListHistoricPlayers(parent, LOC("HISTTRK"), {}, NOLABEL, CMDFLAGS_LIST | CMDFLAG_FEATURELIST_ULTIMATE)
	{
	}

	void CommandTrackedPlayers::onClick(Click& click)
	{
		if (click.isUltimateEdition())
		{
			return CommandList::onClick(click);
		}
		onClickImplUnavailable(click);
	}

	std::vector<CommandHistoricPlayer*> CommandTrackedPlayers::collectPlayers() const
	{
		std::vector<CommandHistoricPlayer*> res{};
		for (const auto& _cmd : PlayerHistory::player_history_command->children)
		{
			if (_cmd->type == COMMAND_LIST_HISTORICPLAYER)
			{
				auto cmd = _cmd->as<CommandHistoricPlayer>();
				if (cmd->player->flags & HP_TRACK)
				{
					res.emplace_back(cmd);
				}
			}
		}
		return res;
	}
}
