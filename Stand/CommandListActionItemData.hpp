#pragma once

#include "CommandPerm.hpp"
#include "Label.hpp"

namespace Stand
{
	struct CommandListActionItemData
	{
		long long value;

		Label menu_name;
		std::vector<CommandName> command_suffixes;
		Label help_text;
		CommandPerm perm;
		commandflags_t flags;

		Label category;
		bool entity_preview; // Assumes 'value' is the entity model hash.

		CommandListActionItemData(long long value, Label menu_name, std::vector<CommandName> command_suffixes = {}, Label help_text = NOLABEL, CommandPerm perm = COMMANDPERM_USERONLY, commandflags_t flags = 0, Label category = NOLABEL, bool entity_preview = false)
			: value(value), menu_name(std::move(menu_name)), command_suffixes(std::move(command_suffixes)), help_text(std::move(help_text)), perm(perm), flags(flags), category(std::move(category)), entity_preview(entity_preview)
		{
		}
	};
}
