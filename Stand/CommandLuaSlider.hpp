#pragma once

#include "CommandLuaSliderBase.hpp"

namespace Stand
{
	template <typename T>
	class CommandLuaSlider : public CommandLuaSliderBase<T>
	{
	private:
		const int on_change_ref;

	public:
		explicit CommandLuaSlider(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text, int min_value, int max_value, int default_value, unsigned int step_size, CommandLuaScript* const script, const int on_change_ref)
			: CommandLuaSliderBase<T>(parent, std::move(menu_name), std::move(command_names), std::move(help_text), script, min_value, max_value, default_value, step_size), on_change_ref(on_change_ref)
		{
		}

		void onChange(Click& click, int prev_value) final
		{
			const auto click_type = click.type;
			const auto value = T::value;
			CommandLuaPhysical<T>::lua_data.queueJob(this, click.type, [this, value, prev_value, click_type](lua_State* L, bool direct)
			{
				luaS_invoke_void_variable(L, direct, on_change_ref, value, prev_value, click_type);
			});
		}
	};
}
