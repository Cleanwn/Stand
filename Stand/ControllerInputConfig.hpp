#pragma once

#include <string>

#include "gta_input.hpp"

namespace Stand
{
	struct ControllerInputConfig
	{
		inline static ControlInput menu_open_close_1 = INPUT_FRONTEND_RB;
		inline static ControlInput menu_open_close_2 = INPUT_FRONTEND_RIGHT;
		inline static ControlInput menu_root_up = INPUT_FRONTEND_LT;
		inline static ControlInput menu_root_down = INPUT_FRONTEND_RT;
		inline static ControlInput menu_up = INPUT_FRONTEND_UP;
		inline static ControlInput menu_down = INPUT_FRONTEND_DOWN;
		inline static ControlInput menu_left = INPUT_FRONTEND_LEFT;
		inline static ControlInput menu_right = INPUT_FRONTEND_RIGHT;
		inline static ControlInput menu_click = INPUT_FRONTEND_ACCEPT;
		inline static ControlInput menu_back = INPUT_FRONTEND_CANCEL;
		inline static ControlInput menu_context = INPUT_FRONTEND_X;
		inline static ControlInput command_box = INPUT_FRONTEND_Y;

		[[nodiscard]] static std::string getOpenCloseStringForGame();
		[[nodiscard]] static std::string getOpenCloseStringForUser();
		[[nodiscard]] static std::string getStringForGame(ControlInput control_input);
		[[nodiscard]] static std::string getStringForUser(ControlInput control_input);
		[[nodiscard]] static std::string getLocalisedString(ControlInput control_input);
	};
}
