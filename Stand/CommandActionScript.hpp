#pragma once

#include "CommandAction.hpp"

namespace Stand
{
	class CommandActionScript : public CommandAction
	{
	public:
		explicit CommandActionScript(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names = {}, Label&& help_text = NOLABEL, commandflags_t flags = CMDFLAGS_ACTION_SCRIPT, CommandPerm perm = COMMANDPERM_USERONLY)
			: CommandAction(parent, std::move(menu_name), std::move(command_names), std::move(help_text), flags, perm)
		{
		}

		void onClick(Click& click) final
		{
			click.ensureScriptThread([this](Click& click)
			{
				onClickScriptThread(click);
			});
		}

		virtual void onClickScriptThread(Click& click) = 0;
	};
}
