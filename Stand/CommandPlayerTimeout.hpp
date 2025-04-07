#pragma once

#include "CommandPlayerToggle.hpp"

namespace Stand
{
	class CommandPlayerTimeout : public CommandPlayerToggle
	{
	public:
		explicit CommandPlayerTimeout(CommandList* const parent);

		void onEnable(Click& click) final;
		void onDisable(Click& click) final;
	};
}
