#pragma once

#include "CommandToggle.hpp"

namespace Stand
{
	class CommandGunFreedom : public CommandToggle
	{
	public:
		explicit CommandGunFreedom(CommandList* const parent);

		void onEnable(Click& click) final;
		void onDisable(Click& click) final;
	};
}
