#include "tbToggleableBehaviourScript.hpp"

#include "FiberPool.hpp"
#include "regular_event.hpp"

namespace Stand
{
	void tbToggleableBehaviourScript::onEnable()
	{
		FiberPool::queueJob([this]
		{
			if (handler_active)
			{
				return;
			}
			handler_active = true;
			reScriptTickEvent::registerHandlerInThreadContext([this]
			{
				if (isEnabled())
				{
					onTick();
					return true;
				}
				handler_active = false;
				return false;
			}, "toggleable behaviour");
		});
	}
}
