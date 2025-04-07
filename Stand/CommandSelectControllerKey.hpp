#pragma once

#include "CommandListSelect.hpp"

#include "gta_input.hpp"

namespace Stand
{
	class CommandSelectControllerKey : public CommandListSelect
	{
	public:
		explicit CommandSelectControllerKey(CommandList* const parent, Label&& menu_name, const ControlInput default_value, bool has_none = false)
			: CommandSelectControllerKey(parent, std::move(menu_name), {}, default_value, has_none)
		{
		}

		explicit CommandSelectControllerKey(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, const ControlInput default_value, bool has_none = false);
	};
}
