#pragma once

#include <functional>

namespace Stand
{
	template <class T>
	class CommandLambdaSlider : public T
	{
	private:
		std::function<void(int, Click&)> on_change_impl;

	public:
		explicit CommandLambdaSlider(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text, const int min_value, const int max_value, const int default_value, const int skip, std::function<void(int, Click&)>&& on_change_impl, commandflags_t flags = CMDFLAGS_SLIDER)
			: T(parent, std::move(menu_name), std::move(command_names), std::move(help_text), min_value, max_value, default_value, skip, flags), on_change_impl(std::move(on_change_impl))
		{
		}

		void onChange(Click& click, int prev_value) final
		{
			on_change_impl(((T*)this)->value, click);
		}
	};
}
