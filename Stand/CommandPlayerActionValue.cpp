#include "CommandPlayerActionValue.hpp"

#include <fmt/xchar.h>

#include "get_next_arg.hpp"

namespace Stand
{
	CommandPlayerActionValue::CommandPlayerActionValue(CommandList* const parent, long long min_value, long long max_value, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text, CommandPerm perm)
		: CommandPlayerAction(parent, std::move(menu_name), std::move(command_names), std::move(help_text), perm), min_value(min_value), max_value(max_value)
	{
		mustHaveCommandName();
	}

	std::wstring CommandPlayerActionValue::getCommandSyntax() const
	{
		return command_names.empty() ? std::wstring{} : std::move(LANG_GET_W("CMD").append(L": ").append(cmdNameToUtf16(command_names.at(0))).append(fmt::format(L" [{} {} {}]", min_value, LANG_GET_W("ARGRNGE"), max_value)));
	}

	void CommandPlayerActionValue::onClick(Click& click)
	{
		if (click.inOnline())
		{
			std::wstring prefill{ cmdNameToUtf16(command_names.at(0)) };
			prefill.push_back(L' ');
			prefill.append(fmt::to_wstring(max_value));
			click.showCommandBoxIfPossible(std::move(prefill));
		}
	}

	void CommandPlayerActionValue::onCommand(Click& click, std::wstring& args)
	{
		auto arg = get_next_arg(args);
		if (arg.empty())
		{
			return onClick(click);
		}
		if (click.inOnline())
		{
			long long value;
			try
			{
				value = std::stoll(arg);
			}
			catch (const std::exception&)
			{
				click.setResponse(LOC("INVARG"));
				std::wstring prefill{ cmdNameToUtf16(command_names.at(0)) };
				prefill.push_back(L' ');
				prefill.append(arg);
				click.showCommandBoxIfPossible(std::move(prefill));
				return;
			}
			onCommandRangeCheck(click, value);
		}
	}

	void CommandPlayerActionValue::onCommandRangeCheck(Click& click, long long value)
	{
		if (value >= min_value && value <= max_value)
		{
			onCommandWithValue(click, value);
		}
		else
		{
			click.setResponse(LOC("INVARG"));
			std::wstring prefill{ cmdNameToUtf16(command_names.at(0)) };
			prefill.push_back(L' ');
			prefill.append(fmt::to_wstring(value));
			click.showCommandBoxIfPossible(std::move(prefill));
		}
	}
}
