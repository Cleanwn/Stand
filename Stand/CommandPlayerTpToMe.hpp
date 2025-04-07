#pragma once

#include "CommandPlayerAction.hpp"

namespace Stand
{
	class CommandPlayerTpToMe : public CommandPlayerAction
	{
	public:
		explicit CommandPlayerTpToMe(CommandList* const parent)
			: CommandPlayerAction(parent, LOC("PLYSMMN"), { CMDNAME("summon") }, NOLABEL, COMMANDPERM_RUDE)
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
			auto pos = click.getEffectiveIssuer().getPos();
			queueJob([this, pos]
			{
				for (const auto& p : PP_PTR->getPlayers())
				{
					p.fiberTeleport(pos);
				}
			});
		}
	};
}
