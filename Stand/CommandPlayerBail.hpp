#pragma once

#include "CommandPlayerToggle.hpp"

namespace Stand
{
	class CommandPlayerBail : public CommandPlayerToggle
	{
	public:
		explicit CommandPlayerBail(CommandList* const parent);

		void onEnable(Click& click) final;
	};
}
