#pragma once

#include "CommandSelectControllerKey.hpp"

namespace Stand
{
	class CommandControllerKeyPointer : public CommandSelectControllerKey
	{
	private:
		ControlInput* const ptr;

	public:
		explicit CommandControllerKeyPointer(CommandList* const parent, ControlInput* const ptr, Label&& menu_name, bool has_none = false)
			: CommandSelectControllerKey(parent, std::move(menu_name), *ptr, has_none), ptr(ptr)
		{
		}

		void onChange(Click& click, long long prev_value) final
		{
			*ptr = (ControlInput)value;
		}
	};
}
