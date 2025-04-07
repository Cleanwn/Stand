#include "CommandLuaList.hpp"

namespace Stand
{
	CommandLuaList::CommandLuaList(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text, CommandLuaScript* const script, const int on_click_ref, const int on_back_ref, const int on_active_list_update_ref)
		: CommandLuaPhysical(parent, std::move(menu_name), std::move(command_names), std::move(help_text), script), on_click_ref(on_click_ref), on_back_ref(on_back_ref), on_active_list_update_ref(on_active_list_update_ref)
	{
	}

	void CommandLuaList::onClick(Click& click)
	{
		if (on_click_ref != LUA_NOREF)
		{
			lua_data.queueJob(this, click.type, [this](lua_State* L, bool direct)
			{
				luaS_invoke_void_variable(L, direct, on_click_ref);
			});
		}
		CommandLuaPhysical::onClick(click);
	}

	void CommandLuaList::onBack(ThreadContext thread_context)
	{
		if (on_back_ref != LUA_NOREF)
		{
			lua_data.queueJob(this, CLICK_MENU, [this](lua_State* L, bool direct)
			{
				luaS_invoke_void_variable(L, direct, on_back_ref);
			});
		}
		CommandLuaPhysical::onBack(thread_context);
	}

	void CommandLuaList::onActiveListUpdate()
	{
		if (on_active_list_update_ref != LUA_NOREF)
		{
			lua_data.queueJob(this, CLICK_MENU, [this](lua_State* L, bool direct)
			{
				luaS_invoke_void_variable(L, direct, on_active_list_update_ref);
			});
		}
	}
}
