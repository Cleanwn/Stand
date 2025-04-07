#include "CommandAction.hpp"

namespace Stand
{
	CommandAction::CommandAction(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text, const commandflags_t flags, const CommandPerm perm, std::vector<Hotkey> default_hotkeys, const CommandType type)
		: CommandPhysical(type, parent, std::move(menu_name), std::move(command_names), std::move(help_text), flags, perm, std::move(default_hotkeys))
	{
	}

	std::string CommandAction::getState() const
	{
		return {};
	}

	void CommandAction::setState(Click& click, const std::string& state)
	{
	}

	void CommandAction::applyDefaultState()
	{
	}
}
