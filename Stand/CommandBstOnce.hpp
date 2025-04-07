#pragma once

#include "CommandAction.hpp"

#include "ScriptGlobal.hpp"

namespace Stand
{
	class CommandBstOnce : public CommandAction
	{
	public:
		explicit CommandBstOnce(CommandList* const parent)
			: CommandAction(parent, LOC("GETBST"), { CMDNAME("bstonce"), CMDNAME("getbst") })
		{
		}

		void onClick(Click& click) final
		{
			if (click.inOnline())
			{
				*ScriptGlobal(GLOBAL_BST).as<int*>() |= (1 << 0);
			}
		}
	};
}
