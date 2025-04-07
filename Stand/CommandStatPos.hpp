#pragma once

#include "CommandList.hpp"

#include "Stats.hpp"

namespace Stand
{
	class CommandStatPos : public CommandList
	{
	private:
		Stat& stat;

	public:
		explicit CommandStatPos(CommandList* const parent, Stat& stat);
	};
}