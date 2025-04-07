#pragma once

#include "CommandPlayerAction.hpp"

#include "CommandHistoricPlayer.hpp"
#include "PlayerHistory.hpp"
#include "rlGamerId.hpp"

namespace Stand
{
	class CommandPlayerInhistory : public CommandPlayerAction
	{
	public:
		explicit CommandPlayerInhistory(CommandList* parent)
			: CommandPlayerAction(parent, LOC("PLYINHIST"))
		{
		}

		void onClick(Click& click) final
		{
			if (auto hp = PC_PTR_SINGLE->historic_player)
			{
				PlayerHistory::findCommand(hp)->redirectOpen(click, this);
			}
		}
	};
}
