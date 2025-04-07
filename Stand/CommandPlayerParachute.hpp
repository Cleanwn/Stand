#pragma once

#include "CommandPlayerAction.hpp"

#include "AbstractPlayer.hpp"
#include "AbstractEntity.hpp"
#include "CommandListPlayer.hpp"
#include "PlayerProvider.hpp"

namespace Stand
{
	class CommandPlayerParachute : public CommandPlayerAction
	{
	public:
		explicit CommandPlayerParachute(CommandList* const parent)
			: CommandPlayerAction(parent, LOC("PLYPARA"), { CMDNAME("paragive") }, NOLABEL, COMMANDPERM_FRIENDLY)
		{
		}

		void onClick(Click& click) final
		{
			DEF_P2;
			const auto players = pp->getPlayers(pp->single);
			ensureYieldableScriptThread(click, [=]
			{
				for (const auto& p : players)
				{
					if (p.exists() && p.hasPed())
					{
						p.getPed().giveWeapons({ ATSTRINGHASH("GADGET_PARACHUTE") });
					}
				}
			});
		}
	};
}
