#include "CommandLuaToggle.hpp"

namespace Stand
{
	CommandLuaToggle::CommandLuaToggle(CommandList* parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text, CommandLuaScript* script, int on_change_ref, bool default_on, CommandPerm perm)
		: CommandLuaPhysical(parent, std::move(menu_name), std::move(command_names), std::move(help_text), script, default_on, CMDFLAGS_TOGGLE, perm), on_change_ref(on_change_ref)
	{
	}

	void CommandLuaToggle::onChange(Click& click)
	{
		lua_data.queueJob(this, click.type, [this, on{ m_on }, click_type{ click.type }](lua_State* L, bool direct)
		{
			luaS_invoke_void_variable(L, direct, on_change_ref, m_on, click_type);
		});
	}
}
