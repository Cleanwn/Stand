#pragma once

#include "CommandAction.hpp"

#include "AbstractPlayer.hpp"

namespace Stand
{
	class CommandRemoveBounty : public CommandAction
	{
	public:
		explicit CommandRemoveBounty(CommandList* const parent)
			: CommandAction(parent, LOC("RMBNTY"), { CMDNAME("removebounty"), CMDNAME("completebounty"), CMDNAME("finishbounty") })
		{
		}

		void onClick(Click& click) final
		{
			if (!click.inOnline())
			{
				return;
			}
			click.ensureScriptThread([]
			{
				if (g_player.getBounty().has_value())
				{
					ScriptGlobal(GLOBAL_BOUNTY_TIME_ELAPSED).set(2880000);
				}
			});
		}
	};
}
