#pragma once

#include "CommandAction.hpp"

#include "ContextMenu.hpp"

namespace Stand
{
	class CommandCtxHotkeyRemove : public CommandAction
	{
	public:
		explicit CommandCtxHotkeyRemove(CommandList* parent, CommandPhysical* const target)
			: CommandAction(parent, LOC("HOTKEY_R2"))
		{
			if (!target->command_names.empty())
			{
				CommandName command_name{ CMDNAME("hotkeyremove ") };
				command_name.append(target->command_names.at(0)).push_back(L' ');
				Hotkey hk = *((CommandCtxHotkey*)parent)->hotkey;
				hk.setHoldMode(false); // Command doesn't need the "Hold" part to find the hotkey
				command_name.append(hk.toStringForCommandName());
				command_names = std::vector<CommandName>(1, std::move(command_name));
			}
		}

		void onClick(Click& click) final
		{
			parent->goBackIfActive(click.thread_context);
			auto* target = ContextMenu::getTargetPhysical();
			target->removeHotkey(*((CommandCtxHotkey*)parent)->hotkey, click.type);
			if (target->canCountAsCommandWithHotkeys() && target->hotkeys.empty())
			{
				target->removeFromCommandsWithHotkeys();
			}
			((CommandCtxHotkey*)parent)->save(target);
		}
	};
}
