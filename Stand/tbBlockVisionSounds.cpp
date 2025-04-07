#include "tbBlockVisionSounds.hpp"

#include "NativeTableHooks.hpp"
#include "ScriptGlobal.hpp"
#include "scrNativeCallContext.hpp"

namespace Stand
{
	void tbBlockVisionSounds::onEnable()
	{
		NativeTableHooks::createNativeEntrypointOverride(0x67C540AA08E4A6F5, [](rage::scrNativeCallContext& ctx)
		{
			if (ctx.getArg<const char*>(2) != nullptr
				&& strcmp(ctx.getArg<const char*>(2), "GTAO_Vision_Modes_SoundSet") != 0
				)
			{
				NativeTableHooks::og(0x67C540AA08E4A6F5)(ctx);
			}
		});
	}

	void tbBlockVisionSounds::onDisable()
	{
		NativeTableHooks::removeNativeEntrypointOverride(0x67C540AA08E4A6F5);
	}
}
