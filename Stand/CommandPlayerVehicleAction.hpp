#pragma once

#include "CommandOnPlayerVehicle.hpp"
#include "CommandAction.hpp"

namespace Stand
{
	class CommandPlayerVehicleAction : public CommandOnPlayerVehicle<CommandAction>
	{
	public:
		explicit CommandPlayerVehicleAction(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names = {}, Label&& help_text = NOLABEL, uint8_t pv_flags = 0, CommandPerm perm = COMMANDPERM_USERONLY)
			: CommandOnPlayerVehicle(parent, std::move(menu_name), std::move(command_names), std::move(help_text), CMDFLAGS_ACTION | CMDFLAG_TEMPORARY, perm)
		{
			this->pv_flags = pv_flags;
			PC_PTR->registerCommandNames(this);
		}
	};
}
