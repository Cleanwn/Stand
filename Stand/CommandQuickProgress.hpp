#pragma once

#include "CommandList.hpp"

namespace Stand
{
	class CommandQuickProgress : public CommandList
	{
	public:
		explicit CommandQuickProgress(CommandList* const parent);
	};
}
