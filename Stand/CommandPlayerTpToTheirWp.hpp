#pragma once

#include "CommandPlayerAction.hpp"

#include "Vector2Plus.hpp"

namespace Stand
{
	class CommandPlayerTpToTheirWp : public CommandPlayerAction
	{
	public:
		explicit CommandPlayerTpToTheirWp(CommandList* const parent)
			: CommandPlayerAction(parent, LOC("PLYWPTP"), { CMDNAME("wptp") })
		{
		}

		void onClick(Click& click)
		{
			if (PP_PTR->isUser())
			{
				click.notAvailableOnUser();
				return;
			}
			if (!click.inOnline())
			{
				return;
			}

			const auto plr = AbstractPlayer(getPlayer());
			const auto wp = plr.getWaypoint();

			if (wp.has_value())
			{
				queueJob([wp, plr]
				{
					plr.fiberTeleport(wp.value());
				});
			}
			else
			{
				click.setResponse(LIT(LANG_FMT("PLYWPTP_N", plr.getName())));
			}
		}
	};
}