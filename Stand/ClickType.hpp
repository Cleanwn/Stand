#pragma once

#include <cstdint>

namespace Stand
{
	enum ClickType : uint8_t
	{
		CLICK_MENU = 0b00000, // in-game interface click; stop_input is allowed
		CLICK_COMMAND = 0b00001, // via command box
		CLICK_HOTKEY = 0b00010,

		CLICK_FLAG_AUTO = 0b00100, // auto: be non-interactive
		CLICK_BULK = 0b00100, // state is adjusted for consistency; don't call g_gui.on_command_state_change()
		CLICK_AUTO = 0b00101, // state is adjusted programatically by Stand
		CLICK_SCRIPTED = 0b00111, // state is adjusted programatically by a script

		CLICK_FLAG_CHAT = 0b01000,
		CLICK_CHAT_ALL = 0b01000,
		CLICK_CHAT_TEAM = 0b01001,

		CLICK_FLAG_WEB = 0b10000,
		CLICK_WEB = 0b10000,
		CLICK_WEB_COMMAND = 0b10001,
	};
}
