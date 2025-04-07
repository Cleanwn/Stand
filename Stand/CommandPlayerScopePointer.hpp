#pragma once

#include "CommandPlayerScope.hpp"

namespace Stand
{
	class CommandPlayerScopePointer : public CommandPlayerScope
	{
	public:
		playerscope_t* const ptr;

		explicit CommandPlayerScopePointer(CommandList* parent, playertype_t* const ptr, Label&& menu_name, std::vector<CommandName>&& command_names = {}, Label&& help_text = NOLABEL, commandflags_t flags = CMDFLAGS_LIST_SELECT, commandflags_t item_flags = 0);

		void onChange(Click& click, long long prev_value) final;
	};
}
