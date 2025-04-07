#pragma once

#include "CommandPlayerAction.hpp"

namespace Stand
{
	class CommandPlayerActionAggressiveCustom : public CommandPlayerAction
	{
	public:
		explicit CommandPlayerActionAggressiveCustom(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names = {}, Label&& help_text = NOLABEL, commandflags_t flags = 0)
			: CommandPlayerAction(parent, std::move(menu_name), std::move(command_names), std::move(help_text), COMMANDPERM_AGGRESSIVE, flags)
		{
		}
	};
}
