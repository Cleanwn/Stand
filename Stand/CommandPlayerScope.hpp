#pragma once

#include "CommandListSelect.hpp"

#include "PlayerScope.hpp"

namespace Stand
{
	class CommandPlayerScope : public CommandListSelect
	{
	public:
		explicit CommandPlayerScope(CommandList* parent, Label&& menu_name, std::vector<CommandName>&& command_names = {}, Label&& help_text = NOLABEL, playerscope_t default_value = PlayerScope::NOONE, commandflags_t flags = CMDFLAGS_LIST_SELECT, commandflags_t item_flags = 0);
	};
}
