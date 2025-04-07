#pragma once

#include "CommandToggle.hpp"

namespace Stand
{
	class CommandFocusPlayerBeaconSingular : public CommandToggle
	{
	public:
		explicit CommandFocusPlayerBeaconSingular(CommandList* const parent);

		void onChange(Click& click) final;
	};
}
