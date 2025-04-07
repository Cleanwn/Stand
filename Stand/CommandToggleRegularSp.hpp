#pragma once

#include "CommandToggle.hpp"

namespace Stand
{
	class CommandToggleRegularSp : public CommandToggle
	{
	public:
		explicit CommandToggleRegularSp(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names = {}, Label&& help_text = NOLABEL, const bool default_on = false)
			: CommandToggle(parent, std::move(menu_name), std::move(command_names), std::move(help_text), default_on)
		{
		}

		bool checkPermissions(Click& click) noexcept;
		[[nodiscard]] static bool checkPermissions() noexcept;
	};
}
