#pragma once

#include "CommandAction.hpp"

#include "AbstractPlayer.hpp"
#include "PlayerHistory.hpp"

namespace Stand
{
#pragma pack(push, 1)
	class CommandHistoryDelete : public CommandAction
	{
	public:
		explicit CommandHistoryDelete(CommandList* const parent)
			: CommandAction(parent, LOC("DEL"))
		{
		}

		void onClick(Click& click) final
		{
			showWarning(click, getGenericIrreversableWarningLabel(), WARNING_COMMAND_BOUND, [this, click](ThreadContext)
			{
				if (this->parent->as<CommandHistoricPlayer>()->player->inSession().isValid())
				{
					click.setResponse(LOC("PLYHIST_DEL_E"));
				}
				else
				{
					PlayerHistory::erase(this->parent->as<CommandHistoricPlayer>()->player);
				}
			});
		}
	};
#pragma pack(pop)
}
