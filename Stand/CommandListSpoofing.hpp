#pragma once

#include "CommandList.hpp"

namespace Stand
{
	class CommandListSpoofing : public CommandList
	{
	public:
		explicit CommandListSpoofing(CommandList* const parent);
	};
}
