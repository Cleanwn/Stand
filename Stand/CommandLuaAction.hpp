#pragma once

#include "CommandLua.hpp"
#include "CommandAction.hpp"

namespace Stand
{
	class CommandLuaAction : public CommandLuaPhysical<CommandAction>
	{
	private:
		const int on_click_ref;
		const int on_command_ref;
		const std::wstring syntax;

	public:
		explicit CommandLuaAction(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text, CommandLuaScript* const script, const int on_click_ref, const int on_command_ref, std::wstring&& syntax, CommandPerm perm);

		std::wstring getCommandSyntax() const final;

		void onClick(Click& click) final;

		void onCommand(Click& click, std::wstring& args) final;
	};
}
