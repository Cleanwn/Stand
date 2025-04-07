#include "CommandHistoricPlayerWhitelist.hpp"

#include "CommandHistoricPlayer.hpp"
#include "PlayerHistory.hpp"
#include "Hooking.hpp"
#include "lang.hpp"

namespace Stand
{
	CommandHistoricPlayerWhitelist::CommandHistoricPlayerWhitelist(CommandList* parent)
		: CommandToggleNoCorrelation(parent, LOC("WHTJOIN"), {}, LOC("WHTJOIN_H"), false, CMDFLAGS_TOGGLE_NO_CORRELATION | CMDFLAG_TEMPORARY)
	{
	}

	void CommandHistoricPlayerWhitelist::onEnable(Click& click)
	{
		parent->as<CommandHistoricPlayer>()->player->flags |= HP_WHITELIST;
		PlayerHistory::save();
	}

	void CommandHistoricPlayerWhitelist::onDisable(Click& click)
	{
		parent->as<CommandHistoricPlayer>()->player->flags &= ~HP_WHITELIST;
		PlayerHistory::save();
	}

	void CommandHistoricPlayerWhitelist::onTickInGameViewport()
	{
		setOnIndication(parent->as<CommandHistoricPlayer>()->player->flags & HP_WHITELIST);
	}

	void CommandHistoricPlayerWhitelist::onTickInWebViewport()
	{
		return onTickInWebViewportImplRedirect();
	}
}
