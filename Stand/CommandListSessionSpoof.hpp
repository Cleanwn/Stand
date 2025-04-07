#pragma once

#include "CommandList.hpp"

namespace Stand
{
	class CommandListSessionSpoof : public CommandList
	{
	public:
		explicit CommandListSessionSpoof(CommandList* parent);
	};
}
