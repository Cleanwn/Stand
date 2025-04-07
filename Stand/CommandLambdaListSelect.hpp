#pragma once

#include "CommandListSelect.hpp"

namespace Stand
{
	class CommandLambdaListSelect : public CommandListSelect
	{
	private:
		const std::function<void(long long, Click&)> on_change;

	public:
		explicit CommandLambdaListSelect(CommandList* parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text, std::function<void(long long, Click&)>&& on_change, std::vector<CommandListActionItemData>&& options, long long default_value, const commandflags_t flags = CMDFLAGS_LIST_SELECT, const commandflags_t item_flags = 0)
			: CommandListSelect(parent, std::move(menu_name), std::move(command_names), std::move(help_text), std::move(options), default_value, flags, item_flags), on_change(std::move(on_change))
		{
		}

		void onChange(Click& click, long long prev_value) final
		{
			return on_change(value, click);
		}
	};
}
