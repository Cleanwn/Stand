#pragma once

#include "CommandList.hpp"

namespace Stand
{
	class CommandRidTools : public CommandList
	{
	public:
		explicit CommandRidTools(CommandList* const parent);
	};
}
