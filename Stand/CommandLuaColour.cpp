#include "CommandLuaColour.hpp"

namespace Stand
{
	CommandLuaColour::CommandLuaColour(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text, const DirectX::SimpleMath::Color& default_rgba, bool transparency, CommandLuaScript* const script, const int on_change_ref)
		: CommandLuaPhysical(parent, std::move(menu_name), std::move(command_names), std::move(help_text), script, default_rgba, transparency), on_change_ref(on_change_ref)
	{
	}

	void CommandLuaColour::onChange(Click& click)
	{
		const auto click_type = click.type;
		lua_data.queueJob(this, click.type, [this, click_type](lua_State* L, bool direct)
		{
			luaS_invoke_void_variable(L, direct, on_change_ref, getRGBA(), click_type);
		});
	}
}
