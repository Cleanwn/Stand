#pragma once

#include "CommandTab.hpp"

namespace Stand
{
	class CommandTabSelf : public CommandTab
	{
	public:
		explicit CommandTabSelf(CommandList* const parent);

		void populate();
	};
}
