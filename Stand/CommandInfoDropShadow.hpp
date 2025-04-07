#pragma once

#include "CommandSlider.hpp"

#include "drawDebugText.hpp"

namespace Stand
{
	class CommandInfoDropShadow : public CommandSlider
	{
	public:
		explicit CommandInfoDropShadow(CommandList* const parent)
			: CommandSlider(parent, LOC("DRPSHDW"), { CMDNAME("infodropshadow") }, NOLABEL, 0, 255, 1)
		{
		}

		void onFocus(ThreadContext thread_context, Direction momentum) final
		{
			g_force_debug_text = true;
		}

		void onBlur(ThreadContext thread_context, Direction momentum) final
		{
			g_force_debug_text = false;
		}

		void onChange(Click& click, int prev_value) final
		{
			if (g_gui.canUpdateThemeSetting())
			{
				g_renderer.info_drop_shadow = (uint8_t)value;
			}
		}
	};
}
