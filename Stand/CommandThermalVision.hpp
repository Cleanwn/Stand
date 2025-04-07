#pragma once

#include "CommandToggle.hpp"

#include "tbBlockVisionSounds.hpp"
#include "natives.hpp"
#include "NativeTableHooks.hpp"

namespace Stand
{
	class CommandThermalVision : public CommandToggle
	{
	public:
		explicit CommandThermalVision(CommandList* const parent)
			: CommandToggle(parent, LOC("THERV"), { CMDNAME("thermalvision") })
		{
		}

		void onEnable(Click& click) final
		{
			g_tb_block_vision_sounds.enable();
			NativeTableHooks::createNativeEntrypointOverride(0x7E08924259E08CE0, &nullsub<rage::scrNativeCallContext&>);
			registerScriptTickEventHandler(click, [this]
			{
				if (!m_on)
				{
					NativeTableHooks::removeNativeEntrypointOverride(0x7E08924259E08CE0);
					GRAPHICS::SET_SEETHROUGH(false);
					g_tb_block_vision_sounds.disable();
					return false;
				}
				if (!GRAPHICS::GET_USINGSEETHROUGH())
				{
					GRAPHICS::SET_SEETHROUGH(true);
				}
				return true;
			});
		}
	};
}
