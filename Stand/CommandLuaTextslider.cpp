#include "CommandLuaTextslider.hpp"

namespace Stand
{
	CommandLuaTextslider::CommandLuaTextslider(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text, CommandLuaScript* const script, std::vector<CommandTextsliderData>&& options, const int on_click_ref)
		: CommandLuaPhysical(parent, std::move(menu_name), std::move(command_names), std::move(help_text), script, std::move(options)), on_click_ref(on_click_ref)
	{
	}

	void CommandLuaTextslider::onClick(Click& click)
	{
		if (hasOptions())
		{
			const auto click_type = click.type;
			lua_data.queueJob(this, click.type, [this, click_type](lua_State* L, bool direct)
			{
				luaS_invoke_void_variable(L, direct, on_click_ref, value, getCurrentValueLabel().getEnglishUtf8(), click_type);
			});
		}
	}
}
