#include "CommandDisableOutfitRestrictions.hpp"

#include <Windows.h>

#include "NativeTableHooks.hpp"
#include "scrNativeCallContext.hpp"

namespace Stand
{
	CommandDisableOutfitRestrictions::CommandDisableOutfitRestrictions(CommandList* const parent)
		: CommandToggle(parent, LOC("DOTFTRESTR"), combineCommandNames(CMDNAMES("no", "block", "disable"), CMDNAME("outfitrestrictions")), LOC("DOTFTRESTR_H"))
	{
	}

	void CommandDisableOutfitRestrictions::onEnable(Click& click)
	{
		NativeTableHooks::createNativeEntrypointOverride(0x341DE7ED1D2A1BFD, [](rage::scrNativeCallContext& ctx)
		{
			ctx.setReturnValue<BOOL>(FALSE);
		});
		NativeTableHooks::createNativeEntrypointOverride(0x7796B21B76221BC5, [](rage::scrNativeCallContext& ctx)
		{
			ctx.setReturnValue<BOOL>(FALSE);
		});
		NativeTableHooks::createNativeEntrypointOverride(0xD726BAB4554DA580, [](rage::scrNativeCallContext& ctx)
		{
			ctx.setReturnValue<BOOL>(FALSE);
		});
	}

	void CommandDisableOutfitRestrictions::onDisable(Click& click)
	{
		NativeTableHooks::removeNativeEntrypointOverride(0x341DE7ED1D2A1BFD);
		NativeTableHooks::removeNativeEntrypointOverride(0x7796B21B76221BC5);
		NativeTableHooks::removeNativeEntrypointOverride(0xD726BAB4554DA580);
	}
}
