#pragma once

#include "CommandList.hpp"

namespace Stand
{
	class CommandListTransform : public CommandList
	{
	public:
		explicit CommandListTransform(CommandList* const parent);
	};
}
