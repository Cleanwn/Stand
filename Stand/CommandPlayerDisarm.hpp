#pragma once

#include "CommandPlayerToggle.hpp"

#include "AbstractPlayer.hpp"
#include "CommandListPlayer.hpp"
#include "PlayerProvider.hpp"

namespace Stand
{
	class CommandPlayerDisarm : public CommandPlayerToggle
	{
	public:
		explicit CommandPlayerDisarm(CommandList* const parent)
			: CommandPlayerToggle(parent, LOC("PNSH_UNRM"), { CMDNAME("disarm"), CMDNAME("dearm") }, LOC("PNSH_UNRM_H"), COMMANDPERM_RUDE)
		{
		}

		void onEnable(Click& click) final
		{
			PP_PTR->toggleBatch(getPathEnFromParent(COMMAND_LIST_PLAYER), [](const std::vector<AbstractPlayer>& players)
			{
				for (const AbstractPlayer p : players)
				{
					AbstractEntity ped = p.getPed();
					if (ped.isValid())
					{
						ped.removeAllWeapons();
					}
				}
			});
		}
	};
}
