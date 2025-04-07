#pragma once

#include "CommandIssuable.hpp"

#include "get_next_arg.hpp"
#include "StringUtils.hpp"

namespace Stand
{
	class CommandHotkeyRemove : public CommandIssuable
	{
	public:
		explicit CommandHotkeyRemove(CommandList* parent)
			: CommandIssuable(parent, { CMDNAME("hotkeyremove"), CMDNAME("removehotkey") })
		{
		}

		void onCommand(Click& click, std::wstring& args) final
		{
			auto* target = click.getCommandPhysicalFromArg(get_next_arg(args));
			if (target != nullptr)
			{
				Hotkey hotkey(StringUtils::utf16_to_utf8(args));
				args.clear();
				if (hotkey.operator bool())
				{
					if (target->hasHotkey(hotkey) && target->canHotkeyBeRemoved(hotkey))
					{
						target->removeHotkey(hotkey, click.type);
						g_gui.hotkeys.save();
					}
				}
				return;
			}
			args.clear();
		}
	};
}
