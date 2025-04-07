#pragma once

#include "CommandToggle.hpp"

namespace Stand
{
	class CommandShowTabLicon : public CommandToggle
	{
	public:
		explicit CommandShowTabLicon(CommandList* const parent)
			: CommandToggle(parent, LOC("SHWLICON"), CMDNAMES("tabslefticon"))
		{
		}

		void onEnable(Click& click) final
		{
			CommandTab::render_mode |= TABRENDER_LICON;
			g_gui.onTabRenderModeUpdate();
		}

		void onDisable(Click& click) final
		{
			CommandTab::render_mode &= ~TABRENDER_LICON;
			g_gui.onTabRenderModeUpdate();
		}
	};
}
