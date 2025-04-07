#pragma once

#include "CommandAction.hpp"

#include "get_next_arg.hpp"
#include "PlayerHistory.hpp"
#include "ScAccount.hpp"
#include "StringUtils.hpp"

namespace Stand
{
	class CommandHistoryAddName : public CommandAction
	{
	public:
		explicit CommandHistoryAddName(CommandList* parent)
			: CommandAction(parent, LOC("HISTADD"), { CMDNAME("historyadd") })
		{
		}

		std::wstring getCommandSyntax() const final
		{
			return std::move(LANG_GET_W("CMD").append(L": ").append(cmdNameToUtf16(command_names.at(0))).append(L" ").append(LANG_GET_W("ARGNME")));
		}

		void onCommand(Click& click, std::wstring& args) final
		{
			if (ScAccount::name2rid(click, StringUtils::utf16_to_utf8(get_next_arg(args)), &PlayerHistory::manual_add_complete))
			{
				return onClick(click);
			}
		}
	};
}
