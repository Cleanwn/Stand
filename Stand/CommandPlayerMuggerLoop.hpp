#pragma once

#include "CommandPlayerToggle.hpp"

namespace Stand
{
	class CommandPlayerMuggerLoop : public CommandPlayerToggle
	{
	public:
		explicit CommandPlayerMuggerLoop(CommandList* const parent);

		void onEnable(Click& click) final;
		void onDisable(Click& click) final;
	};
}
