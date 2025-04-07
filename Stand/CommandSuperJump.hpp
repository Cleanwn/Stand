#pragma once

#include "CommandTogglePointer.hpp"

#include "TickMgr.hpp"

namespace Stand
{
	class CommandSuperJump : public CommandTogglePointer
	{
	public:
		explicit CommandSuperJump(CommandList* const parent)
			: CommandTogglePointer(parent, &TickMgr::super_jump.every_tick, LOC("FROG"), { CMDNAME("superjump"), CMDNAME("frogmode") }, LOC("FROG_H"))
		{
		}
	};
}
