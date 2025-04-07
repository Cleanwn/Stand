#include "CommandPlayerKickLoveLetter.hpp"

#include "LeaveReasons.hpp"
#include "netPeerId.hpp"
#include "pointers.hpp"

namespace Stand
{
	CommandPlayerKickLoveLetter::CommandPlayerKickLoveLetter(CommandList* const parent)
		: CommandPlayerActionAggressive(parent, LOC("PLYKCK_L"), CMDNAMES_OBF("loveletterkick", "loveletter"), Label::combineWithSpace(LOC("PLYKCK_H_DSCRT"), LOC("PLYKCK_H_B_H")))
	{
	}

	Label CommandPlayerKickLoveLetter::getActivationName() const
	{
		return LIT(LANG_FMT("PLY_A", fmt::arg("command", LANG_GET("PLYKCK_L_G")), fmt::arg("player", getPlayerName())));
	}

	void CommandPlayerKickLoveLetter::onClick(Click& click)
	{
		AbstractPlayer p = PP_PTR->getPlayers(true).at(0);
		if (p == g_player)
		{
			click.notAvailableOnUser();
		}
		else if (p.isHost())
		{
			click.setResponse(LOC("CMDNH"));
		}
		else if (g_player.isHost())
		{
			if (!p.checkAggressiveAction())
			{
				return;
			}
			if (p.canUseUnblockableKick())
			{
				ensureScriptThread(click, [p]
				{
					p.kickHost(AbstractPlayer::DESYNC);
				});
			}
		}
		else if (AbstractPlayer::performing_love_letter_kick)
		{
			click.setResponse(LOC("2FAST"));
		}
		else
		{
			if (!p.checkAggressiveAction())
			{
				return;
			}
			LeaveReasons::getEntry(p.getPeerId()).love_letter_from = g_player;
			p.kickLoveLetter(true);
		}
	}
}
