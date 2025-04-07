#include "CommandPlayerKickHost.hpp"

#include "CommandListPlayer.hpp"
#include "natives.hpp"
#include "PlayerProvider.hpp"
#include "ScriptGlobal.hpp"
#include "ScriptHostUtil.hpp"
#include "Util.hpp"

namespace Stand
{
	CommandPlayerKickHost::CommandPlayerKickHost(CommandList* const parent)
		: CommandPlayerActionAggressive(parent, LOC("PLYKCK_H"), CMDNAMES_OBF("hostkick"), Label::combineWithSpace(LOC("PLYKCK_H_H"), LOC("PLYKCK_H_B_H")))
	{
	}

	void CommandPlayerKickHost::onClick(Click& click)
	{
		PC_PTR->ensureOnlineAndWarnSelfharm(this, click, [this](ThreadContext thread_context)
		{
			DEF_P2;
			std::vector<AbstractPlayer> players = pp->getPlayers(pp->single);
			if (players.size() == 1
				&& !players.at(0).checkAggressiveAction()
				)
			{
				return;
			}
			if (!g_player.isHost())
			{
				for (const auto& p : players)
				{
					p.putInTimeout(thread_context);
				}
			}
			ensureYieldableScriptThread(thread_context, [players{ std::move(players) }]
			{
				for (const auto& p : players)
				{
					if (g_player.isHost())
					{
						if (p.canUseUnblockableKick())
						{
							p.kickHost(AbstractPlayer::VOTE_KICK);
						}
					}
					else
					{
						p.onPreRemove();
						p.kickScriptHost();
					}
				}
			});
		});
	}
}
