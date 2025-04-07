#pragma once

#include "CommandList.hpp"

#include "Stats.hpp"

namespace Stand
{
	class CommandStatDate : public CommandList
	{
	private:
		Stat& stat;

	public:
		explicit CommandStatDate(CommandList* const parent, Stat& stat);
	};
}