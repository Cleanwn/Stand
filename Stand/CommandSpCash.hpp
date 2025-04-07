#pragma once

#include "CommandActionScript.hpp"

#include "get_next_arg.hpp"
#include "lang.hpp"
#include "natives.hpp"
#include "StringUtils.hpp"

namespace Stand
{
	class CommandSpCash : public CommandActionScript
	{
	private:
		const Hash stat_hash;

	public:
		explicit CommandSpCash(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, const Hash stat_hash)
			: CommandActionScript(parent, std::move(menu_name), std::move(command_names)), stat_hash(stat_hash)
		{
		}

		std::wstring getCommandSyntax() const final
		{
			return std::move(LANG_GET_W("CMD").append(L": ").append(cmdNameToUtf16(command_names.at(0))).append(L" [0 ").append(LANG_GET_W("ARGRNGE")).append(L" 2147483647]"));
		}

		void onClickScriptThread(Click& click) final
		{
			click.showCommandBoxIfPossible(std::move(std::wstring(cmdNameToUtf16(command_names.at(0))).append(L" 2147483647")));
		}

		void onCommand(Click& click, std::wstring& args) final
		{
			auto arg = get_next_arg(args);
			if (arg.empty())
			{
				return onClickScriptThread(click);
			}
			try
			{
				const int cash = std::stoi(arg);
				ensureScriptThread(click, [this, cash]
				{
					STATS::STAT_SET_INT(stat_hash, cash, true);
				});
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
