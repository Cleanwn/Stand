#pragma once

#include "CommandInput.hpp"

#include "netIpAddress.hpp"

namespace Stand
{
	class CommandInputIpAddress : public CommandInput
	{
	public:
		explicit CommandInputIpAddress(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text, rage::netIpAddress&& default_value);

		void setIP(Click& click, rage::netIpAddress&& ip_address);

		void onCommand(Click& click, std::wstring& args) final;

		virtual void onChange(Click& click);
	};
}
