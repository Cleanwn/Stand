#include "CommandLuaTextInput.hpp"

namespace Stand
{
	CommandLuaTextInput::CommandLuaTextInput(CommandList* parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text, CommandLuaScript* const script, const int on_change_ref, std::string&& default_value)
		: CommandLuaPhysical(parent, std::move(menu_name), std::move(command_names), std::move(help_text), script, std::move(default_value)), on_change_ref(on_change_ref)
	{
	}

	void CommandLuaTextInput::onCommand(Click& click, std::wstring& args)
	{
		click.setNoResponse();
		onCommandApply(click, args);
		lua_data.queueJob(this, click.type, [
			this,
			value{ this->value }, // if script updates the value every tick, it won't receive the user's actual change if we don't copy it here
			click_type{ click.type }
		](lua_State* L, bool direct)
		{
			luaS_invoke_void_variable(L, direct, on_change_ref, value, click_type);
		});
	}
}
