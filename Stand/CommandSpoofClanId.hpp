#pragma once

#include "CommandInput.hpp"

namespace Stand
{
	class CommandSpoofClanId : public CommandInput
	{
	public:
		explicit CommandSpoofClanId(CommandList* const parent)
			: CommandInput(parent, LIT("ID"), CMDNAMES("crewid"), LOC("SPFCRW_ID_H"), "133742069")
		{
		}

		std::wstring getCommandSyntax() const final
		{
			return std::move(LANG_GET_W("CMD").append(L": ").append(cmdNameToUtf16(command_names.at(0))).append(L" [id]"));
		}

		void onCommand(Click& click, std::wstring& args) final
		{
			auto arg = get_next_arg(args);
			if (arg.empty())
			{
				return onClick(click);
			}
			try
			{
				g_hooking.spoofed_clan_membership.clan.id = std::stoll(arg);
				setValueFromHooking(click.type);
			}
			catch (const std::exception&)
			{
				onCommandInvalidArg(click, arg);
			}
		}

		void setValueFromHooking(ClickType click_type)
		{
			setValue(fmt::to_string(g_hooking.spoofed_clan_membership.clan.id), click_type);
		}
	};
}
