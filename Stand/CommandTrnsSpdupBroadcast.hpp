#pragma once

#include "CommandToggle.hpp"

namespace Stand
{
	class CommandTrnsSpdupBroadcast : public CommandToggle
	{
	public:
		explicit CommandTrnsSpdupBroadcast(CommandList* const parent);

		void onEnable(Click& click) final;
		void onDisable(Click& click) final;
	};
}
