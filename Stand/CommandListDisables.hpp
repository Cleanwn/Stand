#pragma once

#include "CommandList.hpp"

namespace Stand
{
	class CommandListDisables : public CommandList
	{
	public:
		explicit CommandListDisables(CommandList* const parent);
	};
}
