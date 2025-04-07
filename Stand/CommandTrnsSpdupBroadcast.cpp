#include "CommandTrnsSpdupBroadcast.hpp"

#include <Windows.h>

#include "NativeTableHooks.hpp"
#include "scrNativeCallContext.hpp"

namespace Stand
{
	CommandTrnsSpdupBroadcast::CommandTrnsSpdupBroadcast(CommandList* const parent)
		: CommandToggle(parent, LOC("TRNS_SPDUP_BRDCST"), { CMDNAME("skipbroadcast") })
	{
	}

	void CommandTrnsSpdupBroadcast::onEnable(Click& click)
	{
		// NETWORK_HAS_RECEIVED_HOST_BROADCAST_DATA
		NativeTableHooks::createNativeEntrypointOverride(0x5D10B3795F3FC886, [](rage::scrNativeCallContext& ctx)
		{
			return ctx.setReturnValue<>(TRUE);
		});
	}

	void CommandTrnsSpdupBroadcast::onDisable(Click& click)
	{
		NativeTableHooks::removeNativeEntrypointOverride(0x5D10B3795F3FC886);
	}
}
