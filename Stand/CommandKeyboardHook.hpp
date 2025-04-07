#pragma once

#include "CommandToggle.hpp"

#include "pointers.hpp"

namespace Stand
{
	class CommandKeyboardHook : public CommandToggle
	{
	public:
		explicit CommandKeyboardHook(CommandList* const parent)
			: CommandToggle(parent, LOC("NKBDHK"), {}, LOC("NKBDHK_H"))
		{
		}

		void onEnable(Click& click) final
		{
			if (pointers::remove_keyboard_hook)
			{
				pointers::remove_keyboard_hook();
			}
		}

		void onDisable(Click& click) final
		{
			if (pointers::add_keyboard_hook)
			{
				pointers::add_keyboard_hook();
			}
		}
	};
}
