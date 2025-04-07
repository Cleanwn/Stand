#pragma once

#include <cstdint>

#include "Label.hpp"

namespace Stand
{
	using CommandPerm = uint8_t;
#define COMMANDPERM_FRIENDLY	(CommandPerm)0b1
#define COMMANDPERM_NEUTRAL		(CommandPerm)0b10
#define COMMANDPERM_SPAWN		(CommandPerm)0b100
#define COMMANDPERM_RUDE		(CommandPerm)0b1000
#define COMMANDPERM_AGGRESSIVE	(CommandPerm)0b10000
#define COMMANDPERM_TOXIC		(CommandPerm)0b100000
#define COMMANDPERM_USERONLY	(CommandPerm)0b1000000
#define COMMANDPERM_ALL			(CommandPerm)0b1111111
#define COMMANDPERM_ANY			(COMMANDPERM_ALL & ~COMMANDPERM_USERONLY)

	[[nodiscard]] extern Label getCommandPermLabel(CommandPerm perm);
}
