#pragma once

#include "CommandLua.hpp"
#include "CommandToggle.hpp"

namespace Stand
{
	class CommandLuaToggle : public CommandLuaPhysical<CommandToggle>
	{
	private:
		const int on_change_ref;

	public:
		explicit CommandLuaToggle(CommandList* parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text, CommandLuaScript* script, int on_change_ref, bool default_on, CommandPerm perm);

		void onChange(Click& click) final;
	};
}
