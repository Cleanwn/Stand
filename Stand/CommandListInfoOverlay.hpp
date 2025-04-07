#pragma once

#include "CommandList.hpp"

namespace Stand
{
	class CommandListInfoOverlay : public CommandList
	{
	public:
		explicit CommandListInfoOverlay(CommandList* const parent);
	};
}
