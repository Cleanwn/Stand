#pragma once

#include "CommandToggle.hpp"

#include <soup/BitPointer.hpp>

namespace Stand
{
	class CommandToggleBitPointer : public CommandToggle
	{
	private:
		soup::BitPointer ptr;

	public:
		explicit CommandToggleBitPointer(CommandList* const parent, const soup::BitPointer& ptr, Label&& menu_name, std::vector<CommandName>&& command_names = {}, Label&& help_text = NOLABEL, commandflags_t flags = CMDFLAGS_TOGGLE)
			: CommandToggle(parent, std::move(menu_name), std::move(command_names), std::move(help_text), ptr.get(), flags), ptr(ptr)
		{
		}

		void onChange(Click& click) final
		{
			ptr.set(m_on);
		}
	};
}
