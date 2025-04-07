#pragma once

#include "CommandToggle.hpp"

namespace Stand
{
	class CommandFocusPlayerBeaconExcludeMe : public CommandToggle
	{
	public:
		explicit CommandFocusPlayerBeaconExcludeMe(CommandList* const parent);

		void onChange(Click& click) final;
	};
}
