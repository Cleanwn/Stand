#pragma once

#include "CommandPlayerListPlayersShortcut.hpp"

namespace Stand
{
	class CommandPlayerBlame : public CommandPlayerListPlayersShortcut
	{
	public:
		explicit CommandPlayerBlame(CommandList* const parent)
			: CommandPlayerListPlayersShortcut(parent, LOC("PLYBLME"))
		{
		}

		std::string getChild(AbstractPlayer& blamed_on, const std::string& target) const final
		{
			return fmt::format("as {} explode {}", blamed_on.getName(), target);
		}
	};
}
