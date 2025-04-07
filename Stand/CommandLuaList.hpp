#pragma once

#include "CommandLua.hpp"
#include "CommandListConcealer.hpp"

namespace Stand
{
	class CommandLuaList : public CommandLuaPhysical<CommandListConcealer>
	{
	private:
		const int on_click_ref;
		const int on_back_ref;
		const int on_active_list_update_ref;
		
	public:
		explicit CommandLuaList(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text, CommandLuaScript* const script, const int on_click_ref, const int on_back_ref, const int on_active_list_update_ref);

		void onClick(Click& click) final;
		void onBack(ThreadContext thread_context) final;
		void onActiveListUpdate() final;
	};
}
