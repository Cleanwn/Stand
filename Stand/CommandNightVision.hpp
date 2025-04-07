#pragma once

#include "CommandToggle.hpp"

#include "tbBlockVisionSounds.hpp"
#include "natives.hpp"
#include "NativeTableHooks.hpp"

namespace Stand
{
	class CommandNightVision : public CommandToggle
	{
	public:
		explicit CommandNightVision(CommandList* const parent)
			: CommandToggle(parent, LOC("NITEV"), { CMDNAME("nightvision") })
		{
		}

		void onEnable(Click& click) final
		{
			g_tb_block_vision_sounds.enable();
			NativeTableHooks::createNativeEntrypointOverride(0x18F621F7A5B1F85D, &nullsub<rage::scrNativeCallContext&>);
			registerScriptTickEventHandler(click, [this]
			{
				if (!m_on)
				{
					NativeTableHooks::removeNativeEntrypointOverride(0x18F621F7A5B1F85D);
					GRAPHICS::SET_NIGHTVISION(false);
					g_tb_block_vision_sounds.disable();
					return false;
				}
				if (!GRAPHICS::GET_USINGNIGHTVISION() && !GRAPHICS::GET_REQUESTINGNIGHTVISION())
				{
					GRAPHICS::SET_NIGHTVISION(true);
				}
				return true;
			});
		}
	};
}
