#pragma once

#include "CommandTab.hpp"

namespace Stand
{
	class CommandTabVehicle : public CommandTab
	{
	public:
		explicit CommandTabVehicle(CommandList* const parent);

		void populate();
	};
}
