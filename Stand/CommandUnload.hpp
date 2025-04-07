#pragma once

#include "CommandActionScript.hpp"

#include "Gui.hpp"

namespace Stand
{
	class CommandUnload : public CommandActionScript
	{
	public:
		explicit CommandUnload(CommandList* const parent)
			: CommandActionScript(parent, LOC("UNLD"), { CMDNAME("unload") }, LOC("UNLD_H"))
		{
		}

		void onClickScriptThread(Click& click) final
		{
			if (!g_gui.isFullyLoaded())
			{
				click.setResponse(LOC("GENWAIT"));
				return;
			}
			if (click.isMenu())
			{
				showWarning(click, LANG_GET_W("GENWARN"), WARNING_COMMAND_BOUND, [=](ThreadContext thread_context)
				{
					ensureScriptThread(thread_context, [=]
					{
						Click click_(click);
						g_gui.backToNormal(click_);
					});
				});
			}
			else
			{
				g_gui.backToNormal(click);
			}
		}
	};
}
