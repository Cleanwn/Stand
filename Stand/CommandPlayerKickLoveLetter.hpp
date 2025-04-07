#pragma once

#include "CommandPlayerActionAggressive.hpp"

namespace Stand
{
	class CommandPlayerKickLoveLetter : public CommandPlayerActionAggressive
	{
	public:
		explicit CommandPlayerKickLoveLetter(CommandList* const parent);

		Label getActivationName() const final;

		void onClick(Click& click) final;
	};
}
