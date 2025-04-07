#include "CommandPlayerToggleBatch.hpp"

namespace Stand
{
	void CommandPlayerToggleBatch::onEnable(Click& click)
	{
		PP_PTR->toggleBatch(getPathEnFromParent(COMMAND_LIST_PLAYER), [this](const std::vector<AbstractPlayer>& players)
		{
			onTick(players);
		});
	}
}
