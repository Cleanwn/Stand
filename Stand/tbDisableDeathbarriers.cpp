#include "tbDisableDeathbarriers.hpp"

#include "FiberPool.hpp"
#include "regular_event.hpp"
#include "ScriptGlobal.hpp"

namespace Stand
{
	void tbDisableDeathbarriers::onEnable()
	{
		FiberPool::queueJob([this]
		{
			reScriptTickEvent::registerHandlerInThreadContext([this]
			{
				*ScriptGlobal(GLOBAL_DISABLE_DEATH_BARRIERS).as<int*>() = isEnabled();
				return isEnabled();
			}, "toggleable behaviour");
		});
	}
}
