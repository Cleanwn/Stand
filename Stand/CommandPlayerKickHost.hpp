#pragma once

#include "CommandPlayerActionAggressive.hpp"

namespace Stand
{
	class CommandPlayerKickHost : public CommandPlayerActionAggressive
	{
	public:
		explicit CommandPlayerKickHost(CommandList* const parent);

		void onClick(Click& click) final;
	};
}
