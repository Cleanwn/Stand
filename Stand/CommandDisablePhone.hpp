#pragma once

#include "CommandToggle.hpp"

namespace Stand
{
	class CommandDisablePhone : public CommandToggle
	{
	public:
		explicit CommandDisablePhone(CommandList* const parent);

		void onEnable(Click& click) final;
		void onDisable(Click& click) final;
	};
}
