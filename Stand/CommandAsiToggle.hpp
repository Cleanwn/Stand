#pragma once

#include "CommandToggle.hpp"

namespace Stand
{
	class CommandAsiToggle : public CommandToggle
	{
	private:
		const std::function<void(bool)> on_change;

	public:
		explicit CommandAsiToggle(CommandList* parent, std::string&& menu_name, std::vector<CommandName>&& command_names, std::string&& help_text, std::function<void(bool)>&& on_change, bool default_on)
			: CommandToggle(parent, LIT(std::move(menu_name)), std::move(command_names), LIT(std::move(help_text)), default_on), on_change(std::move(on_change))
		{
		}

		void onChange(Click& click) final
		{
			ensureScriptThread(click, [this]
			{
				on_change(m_on);
			});
		}
	};
}
