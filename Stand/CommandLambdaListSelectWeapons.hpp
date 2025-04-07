#pragma once

#include "CommandListSelectWeapons.hpp"

namespace Stand
{
	class CommandLambdaListSelectWeapons : public CommandListSelectWeapons
	{
	private:
		std::function<void(Hash, Hash, Click&)> callback;

	public:
		explicit CommandLambdaListSelectWeapons(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text, long long default_value, std::function<void(Hash, Hash, Click&)> callback)
			: CommandListSelectWeapons(parent, std::move(menu_name), std::move(command_names), std::move(help_text), default_value), callback(callback)
		{
		}

		void onChange(Click& click, long long prev_value) final
		{
			this->callback((Hash)this->value, (Hash)prev_value, click);
		}
	};
}