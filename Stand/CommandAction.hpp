#pragma once

#include "CommandPhysical.hpp"

namespace Stand
{
	class CommandAction : public CommandPhysical
	{
	public:
		explicit CommandAction(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names = {}, Label&& help_text = NOLABEL, const commandflags_t flags = 0, const CommandPerm perm = COMMANDPERM_USERONLY, std::vector<Hotkey> default_hotkeys = {}, const CommandType type = COMMAND_ACTION);

		[[nodiscard]] std::string getState() const final;
		void setState(Click& click, const std::string& state) final;
		void applyDefaultState() final;
	};
}
