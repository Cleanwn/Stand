#include "CommandAllPlayers.hpp"

#include "lang.hpp"
#include "PlayerProviderAll.hpp"

namespace Stand
{
	CommandAllPlayers::CommandAllPlayers(CommandList* const parent)
		: CommandListPlayer(parent, &_player_provider, LOC("ALLPLY"), CMDNAMES("allplayers"))
	{
		populate(MAX_PLAYERS);
	}

	std::string CommandAllPlayers::getPlayerName() const
	{
		return menu_name.getLocalisedUtf8();
	}

	CommandName CommandAllPlayers::getCommandNamesSuffix() const
	{
		return CMDNAME("all");
	}
}
