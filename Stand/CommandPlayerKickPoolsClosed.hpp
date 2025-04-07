#pragma once

#include "CommandPlayerActionAggressive.hpp"

namespace Stand
{
	class CommandPlayerKickPoolsClosed : public CommandPlayerActionAggressive
	{
	public:
		explicit CommandPlayerKickPoolsClosed(CommandList* const parent);

		void onClick(Click& click) final;
	};
}
