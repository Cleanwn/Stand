#pragma once

#include "CommandAction.hpp"

#include "ContextMenu.hpp"

namespace Stand
{
	class CommandQuickCtxSave : public CommandAction
	{
	public:
		explicit CommandQuickCtxSave(CommandList* const parent)
			: CommandAction(parent, LOC("SSTATE"))
		{
		}

		void onClick(Click& click) final
		{
			if (click.type == CLICK_MENU)
			{
				openHotkeysList(click.thread_context);
			}
			else
			{
				ContextMenu::doSaveState(click);
			}
		}
	};
}
