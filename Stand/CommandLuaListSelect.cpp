#include "CommandLuaListSelect.hpp"

namespace Stand
{
	CommandLuaListSelect::CommandLuaListSelect(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text, CommandLuaScript* const script, std::vector<CommandListActionItemData>&& options, long long default_value, const int on_change_ref)
		: CommandLuaPhysical(parent, std::move(menu_name), std::move(command_names), std::move(help_text), script, std::move(options), default_value), on_change_ref(on_change_ref)
	{
	}

	void CommandLuaListSelect::onChange(Click& click, long long prev_value)
	{
		const auto click_type = click.type;
		lua_data.queueJob(this, click.type, [this, prev_value, click_type](lua_State* L, bool direct)
		{
			luaS_invoke_void_variable(L, direct, on_change_ref, value, getCurrentValueMenuName().getEnglishUtf8(), prev_value, click_type);
		});
	}
}
