#pragma once

#include "CommandSlider.hpp"

#include "drawDebugText.hpp"

namespace Stand
{
	class CommandInfoScale : public CommandSlider
	{
	public:
		explicit CommandInfoScale(CommandList* const parent)
			: CommandSlider(parent, LOC("SCLE"), { CMDNAME("infoscale") }, NOLABEL, 0, 10000, 19)
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
				g_renderer.info_scale = float(value * TEXT_HEIGHT_PX * 2.0);
			}
		}
	};
}
