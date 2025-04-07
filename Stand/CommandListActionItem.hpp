#pragma once

#include "CommandActionSuffix.hpp"

namespace Stand
{
	class CommandListActionItem : public CommandActionSuffix
	{
	public:
		const long long value;

		explicit CommandListActionItem(CommandList* const parent, long long value, Label&& menu_name, const std::vector<CommandName>& command_suffixes, Label&& help_text, commandflags_t flags, const CommandPerm perm);

		[[nodiscard]] CommandListAction* getListAction() const;

		void onClick(Click& click) final;
	};
}
