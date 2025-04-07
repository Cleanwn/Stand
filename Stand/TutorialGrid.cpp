#include "TutorialGrid.hpp"

#include <fmt/format.h>

#include "ControllerInputConfig.hpp"
#include "fmt_arg.hpp"
#include "GridItemPrimaryText.hpp"
#include "GridItemText.hpp"
#include "huddecl.hpp"
#include "input.hpp"
#include "lang.hpp"
#include "StringUtils.hpp"
#include "Tutorial.hpp"

namespace Stand
{
	constexpr auto width = 500;

	TutorialGrid::TutorialGrid()
		: Grid((HUD_WIDTH / 2) - (width / 2), (HUD_HEIGHT / 2) - 50, 3)
	{
	}

	[[nodiscard]] static std::string getCurrentInstruction()
	{
		switch (Tutorial::state)
		{
		case TUT_OPEN:
			if (Input::keyboard_and_mouse)
			{
				if (Input::user_has_numpad || !Input::scheme.isDefault())
				{
					return LANG_FMT("TUT_OPEN", Input::getPreferedHotkey(Input::scheme.key_menu_open_close).toBracketedString());
				}
				else
				{
					return LANG_FMT("TUT_OPEN2", FMT_ARG("numpad", Input::vk_to_string(VK_ADD)), FMT_ARG("alt", Input::vk_to_string(VK_F4)));
				}
			}
			else
			{
				return LANG_FMT("TUT_OPEN", ControllerInputConfig::getOpenCloseStringForUser());
			}

		case TUT_CLICK:
			return LANG_FMT("TUT_USE", Input::keyboard_and_mouse
				? Input::getPreferedHotkey(Input::scheme.key_menu_click).toBracketedString()
				: ControllerInputConfig::getStringForUser(ControllerInputConfig::menu_click))
				;

		case TUT_BACK:
			return LANG_FMT("TUT_BK", Input::keyboard_and_mouse
				? Input::getPreferedHotkey(Input::scheme.key_menu_back).toBracketedString()
				: ControllerInputConfig::getStringForUser(ControllerInputConfig::menu_back))
				;

		case TUT_DOWN:
			return LANG_FMT("TUT_DN2", Input::keyboard_and_mouse
				? Input::getPreferedHotkey(Input::scheme.key_menu_down).toBracketedString()
				: ControllerInputConfig::getStringForUser(ControllerInputConfig::menu_down))
				;

		case TUT_UP:
			return LANG_FMT("TUT_RUP", Input::keyboard_and_mouse
				? Input::getPreferedHotkey(Input::scheme.key_menu_up).toBracketedString()
				: ControllerInputConfig::getStringForUser(ControllerInputConfig::menu_up))
				;

		case TUT_ROOT:
			return LANG_GET("TUT_TABS");

		case TUT_ROOT_DOWN:
			return LANG_FMT("TUT_RNXT", Input::keyboard_and_mouse
				? Input::getPreferedHotkey(Input::scheme.key_menu_root_down).toBracketedString()
				: ControllerInputConfig::getStringForUser(ControllerInputConfig::menu_root_down))
				;

		case TUT_ROOT_UP:
			return LANG_FMT("TUT_RPRV", Input::keyboard_and_mouse
				? Input::getPreferedHotkey(Input::scheme.key_menu_root_up).toBracketedString()
				: ControllerInputConfig::getStringForUser(ControllerInputConfig::menu_root_up))
				;

		case TUT_CMD:
			return LANG_FMT("TUT_CMD2", Input::getPreferedHotkey(Input::scheme.key_command_box).toBracketedString());

		case TUT_SLIDER:
			return LANG_FMT("TUT_SLIDER",
					Input::keyboard_and_mouse
					? Input::getPreferedHotkey(Input::scheme.key_menu_left).toBracketedString()
					: ControllerInputConfig::getStringForUser(ControllerInputConfig::menu_left),
					Input::keyboard_and_mouse
					? Input::getPreferedHotkey(Input::scheme.key_menu_right).toBracketedString()
					: ControllerInputConfig::getStringForUser(ControllerInputConfig::menu_right)
				);

		case TUT_PROFILES:
			return LANG_GET("TUT_PROFILES");

		case TUT_CTX:
			return LANG_FMT("TUT_CTX2", Input::keyboard_and_mouse
				? Input::getPreferedHotkey(Input::scheme.key_menu_context).toBracketedString()
				: ControllerInputConfig::getStringForUser(ControllerInputConfig::menu_context))
				;
		}
		return "???";
	}

	void TutorialGrid::populate(std::vector<std::unique_ptr<GridItem>>& items_draft)
	{
		items_draft.emplace_back(std::make_unique<GridItemPrimaryText>(width, 24, LANG_GET_W("TUT2")));
		items_draft.emplace_back(std::make_unique<GridItemText>(StringUtils::utf8_to_utf16(getCurrentInstruction()), width, 0, 3));
	}
}
