#pragma once

#include "CommandLuaSliderBase.hpp"

namespace Stand
{
	template <typename T>
	class CommandLuaSliderClick : public CommandLuaSliderBase<T>
	{
	private:
		const int on_click_ref;

	public:
		explicit CommandLuaSliderClick(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text, int min_value, int max_value, int default_value, int step_size, CommandLuaScript* const script, const int on_click_ref)
			: CommandLuaSliderBase<T>(parent, std::move(menu_name), std::move(command_names), std::move(help_text), script, min_value, max_value, default_value, step_size, CMDFLAGS_SLIDER, COMMANDPERM_USERONLY, true), on_click_ref(on_click_ref)
		{
		}

		void onClick(Click& click) final
		{
			auto click_type = click.type;
			CommandLuaPhysical<T>::lua_data.queueJob(this, click.type, [this, click_type](lua_State* L, bool direct)
			{
				luaS_invoke_void_variable(L, direct, on_click_ref, T::value, click_type);
			});
		}
	};
}
