#pragma once

#include "CommandTab.hpp"

namespace Stand
{
	class CommandTabWorld : public CommandTab
	{
	public:
		explicit CommandTabWorld(CommandList* const parent);

		void populate();
	};
}
