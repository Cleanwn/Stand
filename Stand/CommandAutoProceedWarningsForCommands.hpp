#pragma once

#include "CommandToggle.hpp"

namespace Stand
{
	class CommandAutoProceedWarningsForCommands : public CommandToggle
	{
	public:
		explicit CommandAutoProceedWarningsForCommands(CommandList* parent)
			: CommandToggle(parent, LOC("B_CB"), { CMDNAME("commandsskipwarnings"), CMDNAME("commandskipswarnings"), CMDNAME("commandskipwarnings") }, LOC("SUPPGEN_CB_H"))
		{
		}

		void onEnable(Click& click) final
		{
			if (click.isAuto())
			{
				Click::auto_proceed_warnings_for_commands = true;
			}
			else
			{
				auto click_ = click.deriveAuto();
				setStateBool(click_, false);
				showWarning(click, LANG_GET_W("GENWARN"), WARNING_COMMAND_BOUND, [this](ThreadContext thread_context)
				{
					Click click(CLICK_AUTO, thread_context);
					setStateBool(click, true);
				});
			}
		}

		void onDisable(Click& click) final
		{
			Click::auto_proceed_warnings_for_commands = false;
		}
	};
}
