#pragma once

#include "CommandList.hpp"

namespace Stand
{
	class CommandListRadioStations : public CommandList
	{
	public:
		explicit CommandListRadioStations(CommandList* const parent);
	};
}
