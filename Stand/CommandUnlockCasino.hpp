#pragma once

#include "CommandToggle.hpp"

namespace Stand
{
	class CommandUnlockCasino : public CommandToggle
	{
	public:
		explicit CommandUnlockCasino(CommandList* const parent)
			: CommandToggle(parent, LOC("UNKLCSNO"), combineCommandNames(CMDNAMES("no", "block", "disable", "bypass"), CMDNAME("casinoregionlock")))
		{
		}

		void onChange(Click& click) final;
	};
}
