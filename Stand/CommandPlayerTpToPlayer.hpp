#pragma once

#include "CommandPlayerListPlayersShortcut.hpp"

#include "FiberPool.hpp"

namespace Stand
{
	class CommandPlayerTpToPlayer : public CommandPlayerListPlayersShortcut
	{
	public:
		explicit CommandPlayerTpToPlayer(CommandList* const parent)
			: CommandPlayerListPlayersShortcut(parent, LOC("PLYSMMNPLY"), true)
		{
		}

		std::string getChild(AbstractPlayer& blamed_on, const std::string& target) const final
		{
			return fmt::format("as {} summon {}", target, blamed_on.getName());
		}
	};
}
