#include "CommandPlayerKickSmart.hpp"

#include "CommandListPlayer.hpp"
#include "FiberPool.hpp"
#include "lang.hpp"
#include "PlayerProvider.hpp"
#include "Util.hpp"

namespace Stand
{
	CommandPlayerKickSmart::CommandPlayerKickSmart(CommandList* const parent)
		: CommandPlayerActionAggressiveCustom(parent, LOC("PLYKCK_SMRT"), CMDNAMES_OBF("kick"), LOC("PLYKCK_SMRT_H2"))
	{
	}

	Label CommandPlayerKickSmart::getActivationName() const
	{
		return LIT(LANG_FMT("PLY_A", fmt::arg("command", LANG_GET("PLYKCK_SMRT_A")), fmt::arg("player", getPlayerName())));
	}

	void CommandPlayerKickSmart::onClick(Click& click)
	{
		click.setNoResponse();
		PC_PTR->ensureOnlineAndWarnSelfharm(this, click, [this](ThreadContext thread_context)
		{
			DEF_P2;
			const std::vector<AbstractPlayer> players = pp->getPlayers(pp->single);
			if (!players.empty())
			{
				if (players.size() == 1
					&& !players.at(0).checkAggressiveAction()
					)
				{
					return;
				}

				if (g_player.isHost())
				{
					Util::toast(LOC("PLYKCK_T_H"), TOAST_DEFAULT);
				}
				else if (players.size() == 1)
				{
					Util::toast(LOC("PLYKCK_T_C"), TOAST_DEFAULT);
				}
				else
				{
					Util::toast(LOC("PLYKCK_T_C_M"), TOAST_DEFAULT);
				}
				for (const AbstractPlayer& p : players)
				{
					FiberPool::queueJob([p]
					{
						p.kick();
					});
				}
			}
		});
	}
}
