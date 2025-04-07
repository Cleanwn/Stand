#pragma once

#include "CommandPlayerAction.hpp"

namespace Stand
{
	class CommandPlayerCrash : public CommandPlayerAction
	{
	public:
		explicit CommandPlayerCrash(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text = NOLABEL, commandflags_t flags = CMDFLAGS_ACTION);

		void onClick(Click& click) final;
	protected:
		virtual void expandDong(std::vector<AbstractPlayer>&& targets) const = 0;
	};
}
