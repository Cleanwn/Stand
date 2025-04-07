#pragma once

#include "CommandRainbow.hpp"

namespace Stand
{
	class CommandAestheticRainbow : public CommandRainbow
	{
	public:
		explicit CommandAestheticRainbow(CommandList* const parent, CommandColour* colour)
			: CommandRainbow(parent, LOC("LGBT"), { CMDNAME("aestheticrainbow") }, colour)
		{
		}
	};
}
