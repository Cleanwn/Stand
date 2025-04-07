#pragma once

#include "CommandAction.hpp"

namespace Stand
{
	class CommandActionSuffix : public CommandAction
	{
	public:
		const std::vector<CommandName> command_suffixes;

		explicit CommandActionSuffix(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_suffixes, Label&& help_text = NOLABEL, commandflags_t flags = 0, const CommandPerm perm = COMMANDPERM_USERONLY, CommandType type = COMMAND_ACTION);
	private:
		void setCommandNames(CommandListNameshare* base);
	public:
		void updateCommandNames();

		[[nodiscard]] CommandListNameshare* getListNameshare() const;

		std::wstring getCommandSyntax() const final;
	};
}
