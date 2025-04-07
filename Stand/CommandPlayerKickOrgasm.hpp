#pragma once

#include "CommandPlayerActionAggressive.hpp"

namespace Stand
{
	class CommandPlayerKickOrgasm : public CommandPlayerActionAggressive
	{
	public:
		explicit CommandPlayerKickOrgasm(CommandList* const parent)
			: CommandPlayerActionAggressive(parent, LOC("PLYKCK_O"), CMDNAMES_OBF("orgasmkick", "orgasm"), LOC("PLYKCK_H_B_P"))
		{
		}

		void onClick(Click& click) final
		{
			AbstractPlayer p = PP_PTR->getPlayers(true).at(0);
			if (p == g_player)
			{
				click.notAvailableOnUser();
			}
			else if (p == AbstractPlayer::getHost())
			{
				click.setResponse(LOC("CMDNH"));
			}
			else if (p.isOrgasmKickOnCooldown())
			{
				click.setResponse(LOC("2FAST"));
			}
			else
			{
				p.putInTimeout(click.thread_context);
				p.kickOrgasm();
			}
		}
	};
}
