#pragma once

#include "CommandPlayerActionAggressive.hpp"

#include "CommandListPlayer.hpp"
#include "PlayerProvider.hpp"

namespace Stand
{
	class CommandPlayerKickNonHost : public CommandPlayerActionAggressive
	{
	public:
		explicit CommandPlayerKickNonHost(CommandList* const parent)
			: CommandPlayerActionAggressive(parent, LOC("PLYKCK_NH"), CMDNAMES_OBF("nonhostkick"), LOC("PLYKCK_H_B_M"))
		{
		}

		void onClick(Click& click) final
		{
			PC_PTR->ensureOnlineAndWarnSelfharm(this, click, [this](ThreadContext thread_context)
			{
				DEF_P2;
				const std::vector<AbstractPlayer> players = pp->getPlayers(pp->single);
				for (const AbstractPlayer p : players)
				{
					p.putInTimeout(thread_context);
				}
				ensureScriptThread(thread_context, [players{ std::move(players) }]
				{
					for (const AbstractPlayer p : players)
					{
						p.onPreRemove();
						if (p == g_player || players.size() >= 10)
						{
							p.kickBail();
						}
						else
						{
							p.kickNonHost();
						}
					}
				});
			});
		}
	};
}
