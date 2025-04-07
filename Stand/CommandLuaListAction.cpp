#include "CommandLuaListAction.hpp"

#include "CommandListActionItem.hpp"

namespace Stand
{
	CommandLuaListAction::CommandLuaListAction(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text, CommandLuaScript* const script, std::vector<CommandListActionItemData>&& options, const int on_item_click_ref)
		: CommandLuaPhysical(parent, std::move(menu_name), std::move(command_names), std::move(help_text), script, std::move(options)), on_item_click_ref(on_item_click_ref)
	{
	}

	void CommandLuaListAction::onItemClick(Click& click, CommandListActionItem* item)
	{
		CommandLuaPhysical::onItemClick(click, item);
		const auto click_type = click.type;
		lua_data.queueJob(this, click.type, [this, item, click_type](lua_State* L, bool direct)
		{
			luaS_invoke_void_variable(L, direct, on_item_click_ref, item->value, item->menu_name.getEnglishUtf8(), click_type);
		});
	}
}
