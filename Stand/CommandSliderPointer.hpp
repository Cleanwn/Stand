#pragma once

namespace Stand
{
	template <class Super, class Int = int>
	class CommandSliderPointer : public Super
	{
	private:
		Int* const ptr;

	public:
		explicit CommandSliderPointer(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text, Int* const ptr, const int min_value, const int max_value, const int skip = 1, commandflags_t flags = CMDFLAGS_SLIDER)
			: Super(parent, std::move(menu_name), std::move(command_names), std::move(help_text), min_value, max_value, static_cast<int>(*ptr), skip, flags), ptr(ptr)
		{
		}

		void onChange(Click& click, int prev_value) final
		{
			*ptr = static_cast<Int>(reinterpret_cast<Super*>(this)->value);
		}
	};
}
