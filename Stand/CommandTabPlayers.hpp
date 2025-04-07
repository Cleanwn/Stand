#pragma once

#include "CommandTab.hpp"

#include "CommandDivider.hpp"

namespace Stand
{
	class CommandTabPlayers : public CommandTab
	{
	public:
		CommandDivider* divider = nullptr;

		explicit CommandTabPlayers(CommandList* const parent);

		void populate();
	};
}
