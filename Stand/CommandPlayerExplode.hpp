#pragma once

#include "CommandPlayerActionAggressive.hpp"

namespace Stand
{
	class CommandPlayerExplode : public CommandPlayerActionAggressive
	{
	public:
		explicit CommandPlayerExplode(CommandList* parent);

		void onClick(Click& click) final;
	};
}
