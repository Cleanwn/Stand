#include "CommandPlayerIntel.hpp"

#include "AbstractPlayer.hpp"
#include "CommandListPlayer.hpp"
#include "CommandListReadonly.hpp"
#include "PlayerProvider.hpp"

namespace Stand
{
	void CommandPlayerIntel::onTickInGameViewport()
	{
		if (parent->type == COMMAND_LIST_READONLY
			&& !parent->as<CommandListReadonly>()->had_initial_tick
			)
		{
			parent->as<CommandListReadonly>()->doInitialTick();
		}
		else
		{
			const AbstractPlayer p = PP_PTR->getPlayers(true).at(0);
			setValue(getValue(p));
		}
	}

	void CommandPlayerIntel::onTickInWebViewport()
	{
		return onTickInWebViewportImplRedirect();
	}

	void CommandPlayerIntel::onPreScriptedAccess()
	{
		return dispatchOnTickInGameViewportIfNotThisTick();
	}
}
