#pragma once

#include "CommandPlayerActionAggressive.hpp"

#include "CNetworkSession.hpp"
#include "pointers.hpp"

namespace Stand
{
	class CommandPlayerKickBlacklist : public CommandPlayerActionAggressive
	{
	public:
		explicit CommandPlayerKickBlacklist(CommandList* const parent)
			: CommandPlayerActionAggressive(parent, LOC("PLYKCK_BL"), CMDNAMES_OBF("blacklist"), Label::combineWithSpace(LOC("PLYKCK_BL_H"), LOC("PLYKCK_H_HO")))
		{
		}

		Label getActivationName() const final
		{
			return LIT(LANG_FMT("PLY_A", fmt::arg("command", LANG_GET("PLYKCK_BL_A")), fmt::arg("player", getPlayerName())));
		}

		void onClick(Click& click) final
		{
			if (!click.isHost())
			{
				return;
			}
			PC_PTR->ensureOnlineAndWarnSelfharm(this, click, [this](ThreadContext thread_context)
			{
				DEF_P2;
				std::vector<AbstractPlayer> players = pp->getPlayers(pp->single);
				ensureYieldableScriptThread(thread_context, [players{ std::move(players) }]
				{
					std::vector<rage::rlGamerHandle> ghs{};
					for (const auto& p : players)
					{
						if (p.canUseUnblockableKick())
						{
							ghs.emplace_back(p.getGamerHandle());
							p.kickHost(AbstractPlayer::DESYNC);
						}
					}
					for (const auto& gh : ghs)
					{
						(*pointers::network_session)->blacklist.AddGamer(gh);
					}
				});
			});
		}
	};
}
