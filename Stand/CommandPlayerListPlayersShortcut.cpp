#include "CommandPlayerListPlayersShortcut.hpp"

#include <fmt/core.h>

#include "Gui.hpp"

namespace Stand
{
	std::wstring CommandPlayerListPlayersShortcutItem::getCommandSyntax() const
	{
		DEF_P2;
		if (((CommandPlayerListPlayersShortcut*)parent)->getInvalidityReason(pp) != NOLABEL)
		{
			return {};
		}
#ifdef STAND_DEBUG
		SOUP_ASSERT(pp->single); // This command type makes no sense in "All Players"
#endif
		std::string cmd = ((CommandPlayerListPlayersShortcut*)parent)->getChild(pp->getPlayers(true).at(0), this->pp->getCommandName());
		StringUtils::to_lower(cmd);
		cmd.insert(0, ": ");
		cmd.insert(0, LANG_GET("CMD"));
		return StringUtils::utf8_to_utf16(std::move(cmd));
	}

	void CommandPlayerListPlayersShortcutItem::onClick(Click& click)
	{
		DEF_P2;
		Label invalidity_reason = ((CommandPlayerListPlayersShortcut*)parent)->getInvalidityReason(pp);
		if (invalidity_reason != NOLABEL)
		{
			click.setResponse(std::move(invalidity_reason));
			return;
		}
		AbstractPlayer blamed_on = pp->getPlayers(true).at(0);
		for (AbstractPlayer target : this->pp->getPlayers())
		{
			auto click_ = click.derive(CLICK_BULK);
			g_gui.triggerCommands(((CommandPlayerListPlayersShortcut*)parent)->getChild(blamed_on, target.getName()), click_, g_gui.player_list);
		}
	}

	void CommandPlayerListPlayersShortcut::onClick(Click& click)
	{
		DEF_P2;
		Label invalidity_reason = getInvalidityReason(pp);
		if (invalidity_reason != NOLABEL)
		{
			click.setResponse(std::move(invalidity_reason));
		}
		else
		{
			Base::onClick(click);
		}
	}

	Label CommandPlayerListPlayersShortcut::getInvalidityReason(const PlayerProvider* blamed_on) const
	{
		return NOLABEL;
	}
}
