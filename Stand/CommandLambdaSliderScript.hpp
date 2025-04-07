#pragma once

#include <functional>

namespace Stand
{
	template <class T>
	class CommandLambdaSliderScript : public T
	{
	private:
		std::function<void(int, const Click&)> on_change_impl;

	public:
		explicit CommandLambdaSliderScript(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text, const int min_value, const int max_value, const int default_value, const int skip, std::function<void(int, const Click&)>&& on_change_impl)
			: T(parent, std::move(menu_name), std::move(command_names), std::move(help_text), min_value, max_value, default_value, skip), on_change_impl(std::move(on_change_impl))
		{
		}

		void onChange(Click& click, int prev_value) final
		{
			this->ensureScriptThread(click, [this, click]
			{
				on_change_impl(this->value, click);
			});
		}
	};
}
