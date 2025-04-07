#pragma once

#include "CommandToggle.hpp"

namespace Stand
{
	class CommandLambdaToggle : public CommandToggle
	{
	private:
		const std::function<void(bool, Click&)> on_change;

	public:
		explicit CommandLambdaToggle(CommandList* parent, Label&& menu_name, std::vector<CommandName>&& command_names, std::function<void(bool, Click&)>&& on_change, const bool default_on = false, const commandflags_t flags = CMDFLAGS_TOGGLE)
			: CommandToggle(parent, std::move(menu_name), std::move(command_names), NOLABEL, default_on, flags), on_change(std::move(on_change))
		{
		}

		void onChange(Click& click) final
		{
			return on_change(m_on, click);
		}
	};
}
