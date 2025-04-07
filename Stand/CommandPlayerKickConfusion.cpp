#include "CommandPlayerKickConfusion.hpp"

#if HAVE_BREAKUP_KICK

namespace Stand
{
	CommandPlayerKickConfusion::CommandPlayerKickConfusion(CommandList* const parent)
		: CommandPlayerActionAggressive(parent, LOC("PLYKCK_C"), CMDNAMES_OBF("confusionkick"), LOC("PLYKCK_C_H"))
	{
	}

	void CommandPlayerKickConfusion::onClick(Click& click)
	{
		if (click.inOnline() && click.isRegularEdition())
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
			else
			{
				if (!p.checkAggressiveAction())
				{
					return;
				}
				ensureScriptThread(click, [p]
				{
					if (p.canUseUnblockableKick())
					{
						p.onPreRemove();
						p.kickConfusion();
					}
				});
			}
		}
	}
}

#endif
