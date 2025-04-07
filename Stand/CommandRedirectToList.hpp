#pragma once

#include "CommandAction.hpp"

namespace Stand
{
	class CommandRedirectToList : public CommandAction
	{
	public:
		CommandList* target;

		explicit CommandRedirectToList(CommandList* const parent, CommandList* target, Label&& menu_name, Label&& help_text = NOLABEL)
			: CommandAction(parent, std::move(menu_name), {}, std::move(help_text)), target(target)
		{
		}

		void onClick(Click& click) final
		{
			target->redirectOpen(click, this);
		}
	};
}
