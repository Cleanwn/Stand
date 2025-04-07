#pragma once

#include "CommandToggle.hpp"

namespace Stand
{
	template <typename T>
	class CommandToggleBitflag : public CommandToggle
	{
	private:
		T* const addr;
		const T value;

	public:
		explicit CommandToggleBitflag(CommandList* const parent, Label&& menu_name, T* addr, T value)
			: CommandToggle(parent, std::move(menu_name), {}, NOLABEL, *addr & value), addr(addr), value(value)
		{
		}

		void onEnable(Click& click) final
		{
			*addr |= value;
		}

		void onDisable(Click& click) final
		{
			*addr &= ~value;
		}
	};
}

