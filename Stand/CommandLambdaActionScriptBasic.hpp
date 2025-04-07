#pragma once

#include "CommandAction.hpp"

namespace Stand
{
	class CommandLambdaActionScriptBasic : public CommandAction
	{
	private:
		const std::function<void(const Click&)> on_click_impl;

	public:
		explicit CommandLambdaActionScriptBasic(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text, std::function<void(const Click&)>&& on_click_impl)
			: CommandAction(parent, std::move(menu_name), std::move(command_names), std::move(help_text), CMDFLAG_FEATURELIST_BASIC), on_click_impl(std::move(on_click_impl))
		{
		}

		void onClick(Click& click) final
		{
			if (click.isBasicEdition())
			{
				ensureScriptThread(click, [=]
				{
					on_click_impl(click);
				});
			}
		}
	};
}
