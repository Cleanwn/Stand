#pragma once

#include "CommandListSelect.hpp"

namespace Stand
{
	class CommandListSelectPointer : public CommandListSelect
	{
	private:
		long long* ptr;

	public:
		explicit CommandListSelectPointer(CommandList* parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text, std::vector<CommandListActionItemData>&& options, long long* ptr)
			: CommandListSelect(parent, std::move(menu_name), std::move(command_names), std::move(help_text), std::move(options), *ptr), ptr(ptr)
		{
		}

		void onChange(Click& click, long long prev_value) final
		{
			*ptr = value;
		}
	};
}
