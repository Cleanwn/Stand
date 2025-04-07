#pragma once

#include "CommandToggle.hpp"

#include "Gui.hpp"

namespace Stand
{
	class CommandForceWarningRead : public CommandToggle
	{
	public:
		explicit CommandForceWarningRead(CommandList* const parent)
			: CommandToggle(parent, LOC("WARNFRCERD"), {}, NOLABEL, true)
		{
		}
		
		void onEnable(Click& click) final
		{
			g_gui.force_warning_read = true;
		}

		void onDisable(Click& click) final
		{
			if (click.isAuto())
			{
				g_gui.force_warning_read = false;
			}
			else
			{
				auto click_ = click.deriveAuto();
				setStateBool(click_, true);
				showWarning(click, LANG_GET_W("GENWARN"), WARNING_COMMAND_BOUND, [this](ThreadContext thread_context)
				{
					Click click(CLICK_AUTO, thread_context);
					setStateBool(click, false);
				});
			}
		}
	};
}
