#pragma once

#include "CommandToggle.hpp"
#include "CommandOnPlayer.hpp"

namespace Stand
{
	class CommandPlayerToggle : public CommandOnPlayer<CommandToggle>
	{
	public:
		explicit CommandPlayerToggle(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names = {}, Label&& help_text = NOLABEL, const CommandPerm perm = COMMANDPERM_USERONLY, const bool default_on = false);
	};
}
