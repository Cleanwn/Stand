#pragma once

#include "CommandLua.hpp"
#include "CommandListAction.hpp"

namespace Stand
{
	class CommandLuaListAction : public CommandLuaPhysical<CommandListAction>
	{
	private:
		const int on_item_click_ref;

	public:
		explicit CommandLuaListAction(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text, CommandLuaScript* const script, std::vector<CommandListActionItemData>&& options, const int on_item_click_ref);

		void onItemClick(Click& click, CommandListActionItem* item) final;
	};
}
