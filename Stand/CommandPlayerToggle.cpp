#include "CommandPlayerToggle.hpp"

#include "AbstractPlayer.hpp"
#include "CommandListPlayer.hpp"

namespace Stand
{
	CommandPlayerToggle::CommandPlayerToggle(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text, const CommandPerm perm, const bool default_on)
		: CommandOnPlayer(parent, std::move(menu_name), std::move(command_names), std::move(help_text), default_on, CMDFLAGS_TOGGLE, perm)
	{
		PC_PTR->registerCommand(this);
	}
}
