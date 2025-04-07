#pragma once

#include "CommandPlayerToggle.hpp"

namespace Stand
{
	class CommandPlayerAutoheal : public CommandPlayerToggle
	{
	public:
		explicit CommandPlayerAutoheal(CommandList* const parent);

		void onEnable(Click& click) final;
	};
}
