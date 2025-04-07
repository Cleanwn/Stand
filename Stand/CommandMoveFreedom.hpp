#pragma once

#include "CommandToggle.hpp"

namespace Stand
{
	// Current limitations:
	// - Can't duck in stores (although can enter and remain ducked)
	class CommandMoveFreedom : public CommandToggle
	{
	public:
		explicit CommandMoveFreedom(CommandList* parent);

		void onEnable(Click& click) final;
		void onDisable(Click& click) final;
	};
}
