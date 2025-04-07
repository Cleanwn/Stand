#pragma once

#include "CommandToggle.hpp"

namespace Stand
{
	class CommandLambdaToggleScriptTick : public CommandToggle
	{
		using tick_func_t = void(*)();

	private:
		tick_func_t tick_func;

	public:
		explicit CommandLambdaToggleScriptTick(CommandList* const parent, Label&& menu_name, tick_func_t tick_func)
			: CommandToggle(parent, std::move(menu_name)), tick_func(tick_func)
		{
		}

		explicit CommandLambdaToggleScriptTick(CommandList* const parent, Label&& menu_name, Label&& help_text, tick_func_t tick_func)
			: CommandToggle(parent, std::move(menu_name), {}, std::move(help_text)), tick_func(tick_func)
		{
		}

		explicit CommandLambdaToggleScriptTick(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text, tick_func_t tick_func)
			: CommandToggle(parent, std::move(menu_name), std::move(command_names), std::move(help_text)), tick_func(tick_func)
		{
		}

		void onChange(Click& click) final
		{
			registerScriptTickEventHandler(click, [this]
			{
				if (m_on)
				{
					tick_func();
				}
				return m_on;
			});
		}
	};
}
