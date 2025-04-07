#pragma once

#include "CommandTextslider.hpp"
#include "CommandOnPlayerVehicle.hpp"

#include "AbstractEntity.hpp"

namespace Stand
{
	class CommandPlayerVehicleSliderText : public CommandOnPlayerVehicle<CommandTextslider>
	{
	public:
		explicit CommandPlayerVehicleSliderText(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text, std::vector<CommandTextsliderData>&& options, uint8_t flags, CommandPerm perm = COMMANDPERM_USERONLY)
			: CommandOnPlayerVehicle(parent, std::move(menu_name), std::move(command_names), std::move(help_text), std::move(options), CMDFLAGS_SLIDER_TEXT | CMDFLAG_TEMPORARY, perm)
		{
			this->pv_flags = flags;
			PC_PTR->registerCommandNames(this);
		}
	};
}
