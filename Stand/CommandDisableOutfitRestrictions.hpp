#pragma once

#include "CommandToggle.hpp"

namespace Stand
{
	class CommandDisableOutfitRestrictions : public CommandToggle
	{
	public:
		explicit CommandDisableOutfitRestrictions(CommandList* const parent);

		void onEnable(Click& click) final;
		void onDisable(Click& click) final;
	};
}
