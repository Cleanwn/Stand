#pragma once

#include "CommandList.hpp"

namespace Stand
{
#pragma pack(push, 1)
	class CommandMmRegion : public CommandList
	{
	public:
		CommandToggleNoCorrelation* toggle;
		CommandListSelect* select;
		uint32_t og_region = 0;

		explicit CommandMmRegion(CommandList* const parent);
	};
#pragma pack(pop)
}
