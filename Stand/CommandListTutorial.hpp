#pragma once

#include "CommandList.hpp"

namespace Stand
{
	class CommandListTutorial : public CommandList
	{
	public:
		explicit CommandListTutorial(CommandList* const parent);
	};
}
