#pragma once

#include "CommandToggle.hpp"

#include "ConsoleLogger.hpp"
#include "lang.hpp"

namespace Stand
{
	class CommandConsole : public CommandToggle
	{
	public:
		explicit CommandConsole(CommandList* const parent)
			: CommandToggle(parent, LOC("CON"), CMDNAMES("console"))
		{
		}

	public:
		void onChange(Click& click) final
		{
			if (g_console.isInited() != m_on)
			{
				if (m_on)
				{
					if (ConsoleLogger::gameHasAnyConsole())
					{
						auto click_ = click.deriveAuto();
						setStateBool(click_, false);
						showWarning(click, LANG_GET_W("ALOCON_W"), 0, [](ThreadContext thread_context)
						{
							g_console.init();
						});
					}
					else
					{
						g_console.init();
					}
				}
				else if (click.type != CLICK_BULK || !g_gui.about_to_update_root_state)
				{
					g_console.deinit();
				}
			}
		}
	};
}
