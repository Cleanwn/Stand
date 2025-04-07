#include "CommandPlayerScopePointer.hpp"

#include "CommandPlayer.hpp"

namespace Stand
{
	CommandPlayerScopePointer::CommandPlayerScopePointer(CommandList* parent, playertype_t* const ptr, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text, commandflags_t flags, commandflags_t item_flags)
		: CommandPlayerScope(parent, std::move(menu_name), std::move(command_names), std::move(help_text), *ptr, flags, item_flags), ptr(ptr)
	{
	}

	void CommandPlayerScopePointer::onChange(Click& click, long long prev_value)
	{
		*ptr = (playerscope_t)value;
		CommandPlayer::force_flag_update = true;
	}
}
