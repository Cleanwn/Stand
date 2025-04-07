#pragma once

#include "CommandList.hpp"

#include "CommandSchemeKey.hpp"

namespace Stand
{
	class CommandInputScheme : public CommandList
	{
	private:
		CommandSchemeKey* key_menu_open_close = {};
		CommandSchemeKey* key_menu_root_up = {};
		CommandSchemeKey* key_menu_root_down = {};
		CommandSchemeKey* key_menu_up = {};
		CommandSchemeKey* key_menu_down = {};
		CommandSchemeKey* key_menu_left = {};
		CommandSchemeKey* key_menu_right = {};
		CommandSchemeKey* key_menu_click = {};
		CommandSchemeKey* key_menu_back = {};
		CommandSchemeKey* key_menu_context = {};
		CommandSchemeKey* key_command_box = {};

	public:
		explicit CommandInputScheme(CommandList* const parent);

		void setHotkeysFromScheme();
		void onSchemeUpdate();
	};
}
