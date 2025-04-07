#pragma once

#include "CommandInput.hpp"

namespace Stand
{
	class CommandSpoofClanMotto : public CommandInput
	{
	public:
		explicit CommandSpoofClanMotto(CommandList* const parent)
			: CommandInput(parent, LOC("MOTTO"), { CMDNAME("crewmotto") })
		{
		}

		std::wstring getCommandSyntax() const final
		{
			std::wstring syntax{ LANG_GET_W("CMD") };
			syntax.append(L": ").append(cmdNameToUtf16(command_names.at(0))).append(L" [");
			syntax.append(StringUtils::to_lower(LANG_GET_W("MOTTO"))).push_back(L']');
			return syntax;
		}

		void onCommand(Click& click, std::wstring& args) final
		{
			if (checkArgsLength(click, args, 64))
			{
				strcpy(g_hooking.spoofed_clan_membership.clan.motto, StringUtils::utf16_to_utf8(args).c_str());
				if (g_hooking.spoof_clan_membership)
				{
					g_hooking.forceUpdateGamerDataNode();
				}
				setValueFromHooking(click.type);
			}
			args.clear();
		}

		void getExtraInfo(CommandExtraInfo& info, std::wstring& args) final
		{
			CommandInput::getExtraInfo(info, args);
			info.char_limit = 64;
		}

		void setValueFromHooking(ClickType click_type)
		{
			setValue(g_hooking.spoofed_clan_membership.clan.motto, click_type);
		}
	};
}
