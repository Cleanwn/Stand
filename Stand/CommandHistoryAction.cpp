#include "CommandHistoryAction.hpp"

#include "CommandHistoricPlayer.hpp"
#include "FiberPool.hpp"
#include "RemoteGamerConnector.hpp"
#include "Util.hpp"

namespace Stand
{
	CommandHistoryAction::CommandHistoryAction(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text)
		: CommandAction(parent, std::move(menu_name), std::move(command_names), std::move(help_text))
	{
		this->flags |= CMDFLAG_TEMPORARY;
	}

	const CommandHistoricPlayer* CommandHistoryAction::getHpCmd() const noexcept
	{
		return resolveParent(COMMAND_LIST_HISTORICPLAYER)->as<CommandHistoricPlayer>();
	}

	HistoricPlayer* CommandHistoryAction::getHp() const noexcept
	{
		return getHpCmd()->player;
	}
}