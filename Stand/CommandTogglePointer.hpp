#pragma once

#include "CommandToggle.hpp"

namespace Stand
{
	class CommandTogglePointer : public CommandToggle
	{
	protected:
		bool* const ptr;

	public:
		explicit CommandTogglePointer(CommandList* const parent, bool* const ptr, Label&& menu_name, std::vector<CommandName>&& command_names = {}, Label&& help_text = NOLABEL, commandflags_t flags = CMDFLAGS_TOGGLE)
			: CommandToggle(parent, std::move(menu_name), std::move(command_names), std::move(help_text), *ptr, flags), ptr(ptr)
		{
		}

		void onChange(Click& click) final
		{
			*ptr = m_on;
		}
	};
}
