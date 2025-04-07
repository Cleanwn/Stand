#pragma once

#include "CommandAction.hpp"

#include "CNetworkSession.hpp"
#include "pointers.hpp"

namespace Stand
{
	class CommandClearBlacklist : public CommandAction
	{
	public:
		explicit CommandClearBlacklist(CommandList* const parent)
			: CommandAction(parent, LOC("CLRBLKLST"), CMDNAMES_OBF("clearblacklist"), LOC("CLRBLKLST_H"))
		{
		}

		void onClick(Click& click) final
		{
			if (click.isHost())
			{
				for (int64_t i = 1; i != CBlacklistedGamers::MAX_ENTRIES + 1; ++i)
				{
					(*pointers::network_session)->blacklist.AddGamer(rage::rlGamerHandle(i));
				}
			}
		}
	};
}
