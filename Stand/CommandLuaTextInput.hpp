#pragma once

#include "CommandLua.hpp"
#include "CommandInput.hpp"

namespace Stand
{
	class CommandLuaTextInput : public CommandLuaPhysical<CommandInput>
	{
	private:
		const int on_change_ref;

	public:
		explicit CommandLuaTextInput(CommandList* parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text, CommandLuaScript* const script, const int on_change_ref, std::string&& default_value);

		void onCommand(Click& click, std::wstring& args) final;
	};
}
