#pragma once

#include "CommandPlayerToggle.hpp"

#include "AbstractPlayer.hpp"
#include "CommandListPlayer.hpp"
#include "lang.hpp"
#include "natives.hpp"
#include "PlayerProvider.hpp"
#include "Util.hpp"

namespace Stand
{
	class CommandPlayerFreeze : public CommandPlayerToggle
	{
	public:
		explicit CommandPlayerFreeze(CommandList* const parent)
			: CommandPlayerToggle(parent, LOC("FRZ"), { CMDNAME("freeze") }, NOLABEL, COMMANDPERM_AGGRESSIVE)
		{
		}

		void onEnable(Click& click) final
		{
			PP_PTR->toggleBatch(getPathEnFromParent(COMMAND_LIST_PLAYER), [](const std::vector<AbstractPlayer>& players)
			{
				for (const AbstractPlayer& p : players)
				{
					p.sendFreeze();
				}
			});
		}
	};
}
