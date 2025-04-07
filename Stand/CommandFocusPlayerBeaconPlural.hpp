#pragma once

#include "CommandToggle.hpp"

namespace Stand
{
	class CommandFocusPlayerBeaconPlural : public CommandToggle
	{
	public:
		explicit CommandFocusPlayerBeaconPlural(CommandList* const parent);

		void onChange(Click& click) final;
	};
}
