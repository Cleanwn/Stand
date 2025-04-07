#pragma once

#include "CommandActionSuffix.hpp"

namespace Stand
{
	class CommandActionSuffixLambda : public CommandActionSuffix
	{
	private:
		const std::function<void(Click&)> on_click_impl;

	public:
		explicit CommandActionSuffixLambda(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_suffixes, Label&& help_text, std::function<void(Click&)>&& on_click_impl)
			: CommandActionSuffix(parent, std::move(menu_name), std::move(command_suffixes), std::move(help_text)), on_click_impl(std::move(on_click_impl))
		{
		}

		void onClick(Click& click) final
		{
			on_click_impl(click);
		}
	};
}
