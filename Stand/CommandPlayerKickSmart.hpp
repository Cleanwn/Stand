#pragma once

#include "CommandPlayerActionAggressiveCustom.hpp"

namespace Stand
{
	class CommandPlayerKickSmart : public CommandPlayerActionAggressiveCustom
	{
	public:
		explicit CommandPlayerKickSmart(CommandList* const parent);

		Label getActivationName() const final;

		void onClick(Click& click) final;
	};
}
