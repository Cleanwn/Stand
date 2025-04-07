#pragma once

#include "CommandList.hpp"

namespace Stand
{
	class CommandListTextSettings : public CommandList
	{
	public:
		explicit CommandListTextSettings(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, TextSettings* settings);
	};
}
