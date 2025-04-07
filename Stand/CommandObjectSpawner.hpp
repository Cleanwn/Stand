#pragma once

#include "CommandList.hpp"

namespace Stand
{
	class CommandObjectSpawner : public CommandList
	{
	public:
		size_t objects_offset;

		explicit CommandObjectSpawner(CommandList* parent);
	};
}
