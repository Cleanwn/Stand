#pragma once

#include "CommandAction.hpp"

#include "ContextMenu.hpp"

namespace Stand
{
	class CommandQuickCtxRDefault : public CommandAction
	{
	public:
		explicit CommandQuickCtxRDefault(CommandList* const parent)
			: CommandAction(parent, LOC("RGODFLT"))
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
				ContextMenu::doApplyDefaultStateToChildren(click);
			}
		}
	};
}
