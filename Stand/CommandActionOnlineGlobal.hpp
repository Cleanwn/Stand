#pragma once

#include "CommandAction.hpp"

#include "ScriptGlobal.hpp"

namespace Stand
{
	class CommandActionOnlineGlobal : public CommandAction
	{
	private:
		const int global;

	public:
		explicit CommandActionOnlineGlobal(CommandList* const parent, int global, Label&& menu_name, std::vector<CommandName>&& command_names = {}, Label&& help_text = NOLABEL)
			: CommandAction(parent, std::move(menu_name), std::move(command_names), std::move(help_text)), global(global)
		{
		}

		void onClick(Click& click) final
		{
			if (click.inOnline())
			{
				ensureScriptThread(click, [global{ global }]
				{
					*ScriptGlobal(global).as<BOOL*>() = true;
				});
			}
		}
	};
}
