#include "CommandPlayerScope.hpp"

namespace Stand
{
	CommandPlayerScope::CommandPlayerScope(CommandList* parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text, playerscope_t default_value, commandflags_t flags, commandflags_t item_flags)
		: CommandListSelect(parent, std::move(menu_name), std::move(command_names), std::move(help_text), {
			{PlayerScope::NOONE, LOC("DOFF")},
			{PlayerScope::SELF, LOC("PSC_M")},
			{PlayerScope::FRIENDS, LOC("PSC_FM")},
			{PlayerScope::CREW_MEMBERS, LOC("PSC_CFM")},
			{PlayerScope::EVERYONE, LOC("ON")},
		}, default_value, flags, item_flags)
	{
	}
}
