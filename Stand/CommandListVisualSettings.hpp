#pragma once

#include "CommandList.hpp"

namespace Stand
{
	class CommandListVisualSettings : public CommandList
	{
	public:
		explicit CommandListVisualSettings(CommandList* const parent);
	};
}
