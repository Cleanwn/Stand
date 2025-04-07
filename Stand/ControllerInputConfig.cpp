#include "ControllerInputConfig.hpp"

#include <fmt/core.h>

#include "lang.hpp"

namespace Stand
{
	std::string ControllerInputConfig::getOpenCloseStringForGame()
	{
		return fmt::format("~INPUT_{}~ + ~INPUT_{}~", g_control_input_names[ControllerInputConfig::menu_open_close_1], g_control_input_names[ControllerInputConfig::menu_open_close_2]);
	}

	std::string ControllerInputConfig::getOpenCloseStringForUser()
	{
		std::string str = getStringForUser(ControllerInputConfig::menu_open_close_1);
		str.append(" + ");
		str.append(getStringForUser(ControllerInputConfig::menu_open_close_2));
		return str;
	}

	std::string ControllerInputConfig::getStringForGame(ControlInput control_input)
	{
		return fmt::format("~INPUT_{}~", g_control_input_names[control_input]);
	}

	std::string ControllerInputConfig::getStringForUser(ControlInput control_input)
	{
		std::string str = getLocalisedString(control_input);
		str.insert(0, 1, '(');
		str.push_back(')');
		return str;
	}

	std::string ControllerInputConfig::getLocalisedString(ControlInput control_input)
	{
		switch (control_input)
		{
		case INPUT_FRONTEND_ACCEPT: return "A";
		case INPUT_FRONTEND_CANCEL: return "B";
		case INPUT_FRONTEND_X: return "X";
		case INPUT_FRONTEND_Y: return "Y";
		case INPUT_FRONTEND_LB: return LANG_GET("CK_LB");
		case INPUT_FRONTEND_RB: return LANG_GET("CK_RB");
		case INPUT_FRONTEND_LT: return LANG_GET("CK_LT");
		case INPUT_FRONTEND_RT: return LANG_GET("CK_RT");
		case INPUT_FRONTEND_LS: return LANG_GET("CK_LS");
		case INPUT_FRONTEND_RS: return LANG_GET("CK_RS");
		case INPUT_FRONTEND_UP: return LANG_GET("CK_D_U");
		case INPUT_FRONTEND_DOWN: return LANG_GET("CK_D_D");
		case INPUT_FRONTEND_LEFT: return LANG_GET("CK_D_L");
		case INPUT_FRONTEND_RIGHT: return LANG_GET("CK_D_R");
		case INPUT_FRONTEND_PAUSE: return LANG_GET("CK_STRT");
		case INPUT_FRONTEND_SELECT: return LANG_GET("CK_BCK");
		}
		return "???";
	}
}
