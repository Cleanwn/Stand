#pragma once

#include "CommandToggleEditionbound.hpp"

namespace Stand
{
	class CommandToggleRegular : public CommandToggleEditionbound
	{
	public:
		explicit CommandToggleRegular(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names = {}, Label&& help_text = NOLABEL, const bool default_on = false, const std::vector<Hotkey>& default_hotkeys = {})
			: CommandToggleEditionbound(parent, std::move(menu_name), std::move(command_names), std::move(help_text), default_on, CMDFLAGS_TOGGLE | CMDFLAG_FEATURELIST_REGULAR, COMMANDPERM_USERONLY, default_hotkeys)
		{
		}

		bool doPermissionCheck(Click& click) const noexcept final
		{
			return click.isRegularEdition();
		}
	};
}
