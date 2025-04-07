#pragma once

#include "CommandToggle.hpp"

namespace Stand
{
	class CommandDisableDeathbarriers : public CommandToggle
	{
	public:
		explicit CommandDisableDeathbarriers(CommandList* parent);

		void onEnable(Click& click) final;
		void onDisable(Click& click) final;
	};
}
