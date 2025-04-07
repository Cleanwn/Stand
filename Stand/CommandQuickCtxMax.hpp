#pragma once

#include "CommandAction.hpp"

#include "ContextMenu.hpp"

namespace Stand
{
	class CommandQuickCtxMax : public CommandAction
	{
	public:
		explicit CommandQuickCtxMax(CommandList* const parent)
			: CommandAction(parent, LOC("CTX_MAX"))
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
				ContextMenu::doMax(click);
			}
		}
	};
}
