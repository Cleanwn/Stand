#pragma once

#include "CommandPlayerToggleBatch.hpp"

namespace Stand
{
	class CommandPlayerOtr : public CommandPlayerToggleBatch
	{
	public:
		explicit CommandPlayerOtr(CommandList* const parent)
			: CommandPlayerToggleBatch(parent, LOC("OTR"), { CMDNAME("giveotr") }, NOLABEL, COMMANDPERM_FRIENDLY)
		{
		}

		void onTick(const std::vector<AbstractPlayer>& players) const final
		{
			for (AbstractPlayer p : players)
			{
				p.giveOtr();
			}
		}
	};
}
