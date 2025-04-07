#pragma once

#include "CommandPlayerAction.hpp"

#include "Gui.hpp"

namespace Stand
{
	class CommandPlayerTpToMyWp : public CommandPlayerAction
	{
	public:
		explicit CommandPlayerTpToMyWp(CommandList* const parent)
			: CommandPlayerAction(parent, LOC("PLYSMMNWP2"), { CMDNAME("wpsummon") }, NOLABEL)
		{
		}

		void onClick(Click& click) final
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
			auto pos = g_gui.waypoint;
			if (pos.isNull())
			{
				click.setResponse(LOC("BLIPNFND"));
			}
			else
			{
				queueJob([this, pos]
				{
					for (const auto& p : PP_PTR->getPlayers())
					{
						p.fiberTeleport(pos);
					}
				});
			}
		}
	};
}
