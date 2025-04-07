#pragma once

#include "CommandToggleEditionbound.hpp"

namespace Stand
{
	class CommandToggleUltimate : public CommandToggleEditionbound
	{
	public:
		explicit CommandToggleUltimate(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names = {}, Label&& help_text = NOLABEL, const bool default_on = false)
			: CommandToggleEditionbound(parent, std::move(menu_name), std::move(command_names), std::move(help_text), default_on, CMDFLAGS_TOGGLE | (default_on ? 0 : CMDFLAG_FEATURELIST_ULTIMATE))
		{
		}

		bool doPermissionCheck(Click& click) const noexcept final
		{
			return click.isUltimateEdition();
		}
	};
}
