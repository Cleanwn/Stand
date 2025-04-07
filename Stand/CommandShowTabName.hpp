#pragma once

#include "CommandToggle.hpp"

namespace Stand
{
	class CommandShowTabName : public CommandToggle
	{
	public:
		explicit CommandShowTabName(CommandList* const parent)
			: CommandToggle(parent, LOC("SHWNME"), CMDNAMES("tabsname"), NOLABEL, true)
		{
		}

		void onEnable(Click& click) final
		{
			CommandTab::render_mode |= TABRENDER_NAME;
			g_gui.onTabRenderModeUpdate();
		}

		void onDisable(Click& click) final
		{
			CommandTab::render_mode &= ~TABRENDER_NAME;
			g_gui.onTabRenderModeUpdate();
		}
	};
}
