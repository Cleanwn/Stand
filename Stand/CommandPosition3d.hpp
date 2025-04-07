#pragma once

#include "CommandVector3.hpp"

namespace Stand
{
	class CommandPosition3d : public CommandVector3
	{
	public:
		explicit CommandPosition3d(CommandList* parent, Label&& menu_name, std::vector<CommandName>&& command_names);
		void populate();
	};
}
