#pragma once

#include "CommandToggle.hpp"

namespace Stand
{
	class CommandShowTabRicon : public CommandToggle
	{
	public:
		explicit CommandShowTabRicon(CommandList* const parent)
			: CommandToggle(parent, LOC("SHWRICON"), CMDNAMES("tabsrighticon"))
		{
		}

		void onEnable(Click& click) final
		{
			CommandTab::render_mode |= TABRENDER_RICON;
			g_gui.onTabRenderModeUpdate();
		}

		void onDisable(Click& click) final
		{
			CommandTab::render_mode &= ~TABRENDER_RICON;
			g_gui.onTabRenderModeUpdate();
		}
	};
}
