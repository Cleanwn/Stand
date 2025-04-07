#pragma once

#include "CommandIssuable.hpp"

#include "get_next_arg.hpp"
#include "StringUtils.hpp"

namespace Stand
{
	class CommandHotkeyAdd : public CommandIssuable
	{
	public:
		explicit CommandHotkeyAdd(CommandList* parent)
			: CommandIssuable(parent, { CMDNAME("hotkeyadd"), CMDNAME("addhotkey"), CMDNAME("bind") })
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
					target->modifyHotkeys(true, hotkey, click.type);
				}
				return;
			}
			args.clear();
		}
	};
}
