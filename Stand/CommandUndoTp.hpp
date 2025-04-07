#pragma once

#include "CommandActionScript.hpp"

#include "TpUtil.hpp"

namespace Stand
{
	class CommandUndoTp : public CommandActionScript
	{
	public:
		explicit CommandUndoTp(CommandList* const parent)
			: CommandActionScript(parent, LOC("UNDOTP"), { CMDNAME("undoteleport"), CMDNAME("undotp") })
		{
		}

		void onClickScriptThread(Click& click) final
		{
			if (click.canExecuteChainCommand())
			{
				if (TpUtil::undo_stack.empty())
				{
					click.setResponse(LOC("UNDOTP_E"));
				}
				else
				{
					TpUtil::undo_teleport();
					click.setChainCommandExecuted();
				}
			}
		}
	};
}
