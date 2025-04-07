#include "CommandInputIpAddress.hpp"

#include "get_next_arg.hpp"

namespace Stand
{
	CommandInputIpAddress::CommandInputIpAddress(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text, rage::netIpAddress&& default_value)
		: CommandInput(parent, std::move(menu_name), std::move(command_names), std::move(help_text), default_value.operator std::string())
	{
	}

	void CommandInputIpAddress::setIP(Click& click, rage::netIpAddress&& ip_address)
	{
		value = ip_address.operator std::string();
		update_state(click.type);
		onChange(click);
	}

	void CommandInputIpAddress::onCommand(Click& click, std::wstring& args)
	{
		auto arg = get_next_arg(args);
		if (arg.empty())
		{
			return onClick(click);
		}
		try
		{
			setIP(click, rage::netIpAddress::fromString(arg));
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

	void CommandInputIpAddress::onChange(Click& click)
	{
	}
}
