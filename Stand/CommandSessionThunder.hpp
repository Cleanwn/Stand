#pragma once

#include "CommandList.hpp"

namespace Stand
{
	class CommandSessionThunder : public CommandList
	{
	public:
		explicit CommandSessionThunder(CommandList* const parent);
	};
}
