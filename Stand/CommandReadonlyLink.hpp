#pragma once

#include "CommandPhysical.hpp"

namespace Stand
{
	class CommandReadonlyLink : public CommandPhysical
	{
	public:
		const std::string link;

		explicit CommandReadonlyLink(CommandList* const parent, Label&& menu_name, std::string&& link, Label&& help_text = NOLABEL, std::vector<CommandName>&& command_names = {});

		void onClick(Click& click) override;
	};
}
