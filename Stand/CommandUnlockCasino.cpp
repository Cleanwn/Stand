#include "CommandUnlockCasino.hpp"

#include <windows.h>

#include "NativeTableHooks.hpp"
#include "scrNativeCallContext.hpp"

namespace Stand
{
	void _NETWORK_CASINO_CAN_USE_GAMBLING_TYPE(rage::scrNativeCallContext& ctx)
	{
		ctx.setReturnValue<BOOL>(true);
	}

	void CommandUnlockCasino::onChange(Click& click)
	{
		ensureScriptThread(click, [this]
		{
			if (m_on)
			{
				NativeTableHooks::createNativeEntrypointOverride(0x158C16F5E4CF41F8, &_NETWORK_CASINO_CAN_USE_GAMBLING_TYPE);
			}
			else
			{
				NativeTableHooks::removeNativeEntrypointOverride(0x158C16F5E4CF41F8);
			}
		});
	}
}
