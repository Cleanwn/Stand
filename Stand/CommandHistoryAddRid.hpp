#pragma once

#include "CommandAction.hpp"

#include "PlayerHistory.hpp"
#include "ScAccount.hpp"

namespace Stand
{
	class CommandHistoryAddRid : public CommandAction
	{
	public:
		explicit CommandHistoryAddRid(CommandList* parent)
			: CommandAction(parent, LOC("HISTADDRID"), { CMDNAME("historyaddrid") })
		{
		}

		std::wstring getCommandSyntax() const final
		{
			return std::move(LANG_GET_W("CMD").append(L": ").append(cmdNameToUtf16(command_names.at(0))).append(L" [rid]"));
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
				ScAccount::rid2name(click, std::stoull(arg), &PlayerHistory::manual_add_complete);
			}
			catch (const std::exception&)
			{
				click.setResponse(LOC("INVARG"));
				std::wstring prefill{ cmdNameToUtf16(command_names.at(0)) };
				prefill.push_back(L' ');
				prefill.append(arg);
				click.showCommandBoxIfPossible(std::move(prefill));
			}
		}
	};
}
