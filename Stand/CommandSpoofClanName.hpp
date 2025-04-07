#pragma once

#include "CommandInput.hpp"

#include <soup/ObfusString.hpp>

namespace Stand
{
	class CommandSpoofClanName : public CommandInput
	{
	public:
		explicit CommandSpoofClanName(CommandList* const parent)
			: CommandInput(parent, LOC("NME"), { CMDNAME("crewname") }, NOLABEL, soup::ObfusString("Stand.sh"))
		{
		}

		std::wstring getCommandSyntax() const final
		{
			std::wstring syntax{ LANG_GET_W("CMD") };
			syntax.append(L": ").append(cmdNameToUtf16(command_names.at(0))).push_back(L' ');
			syntax.append(LANG_GET_W("ARGNME"));
			return syntax;
		}

		void onCommand(Click& click, std::wstring& args) final
		{
			if (checkArgsLength(click, args, 24))
			{
				strcpy(g_hooking.spoofed_clan_membership.clan.name, StringUtils::utf16_to_utf8(args).c_str());
				setValueFromHooking(click.type);
				if (g_hooking.spoof_clan_membership)
				{
					parent->as<CommandSpoofClan>()->updateData();
				}
			}
			args.clear();
		}

		void getExtraInfo(CommandExtraInfo& info, std::wstring& args) final
		{
			CommandInput::getExtraInfo(info, args);
			info.char_limit = 24;
		}

		void setValueFromHooking(ClickType click_type)
		{
			setValue(g_hooking.spoofed_clan_membership.clan.name, click_type);
		}
	};
}
