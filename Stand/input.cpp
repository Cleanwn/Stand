#include "input.hpp"

#include <fmt/format.h>

#include <soup/joaat.hpp>

#include "AbstractEntity.hpp"
#include "AbstractModel.hpp"
#include "GhostDriver.hpp"
#include "Gui.hpp"
#include "is_number_char.hpp"
#include "lang.hpp"
#include "natives.hpp"
#include "pointers.hpp"
#include "Renderer.hpp"
#include "tbFreecam.hpp"
#include "vk_string_map.hpp"

namespace Stand
{
	bool InputScheme::isDefault() const
	{
		return key_menu_open_close == std::vector<Hotkey>{ VK_F4, VK_ADD }
			|| key_menu_root_up == std::vector<Hotkey>{ VK_RSHIFT, VK_NUMPAD7 }
			|| key_menu_root_down == std::vector<Hotkey>{ VK_RCONTROL, VK_NUMPAD1 }
			|| key_menu_up == std::vector<Hotkey>{ VK_UP, VK_NUMPAD8 }
			|| key_menu_down == std::vector<Hotkey>{ VK_DOWN, VK_NUMPAD2 }
			|| key_menu_left == std::vector<Hotkey>{ VK_LEFT, VK_NUMPAD4 }
			|| key_menu_right == std::vector<Hotkey>{ VK_RIGHT, VK_NUMPAD6 }
			|| key_menu_click == std::vector<Hotkey>{ VK_RETURN, VK_NUMPAD5 }
			|| key_menu_back == std::vector<Hotkey>{ VK_BACK, VK_NUMPAD0 }
			|| key_menu_context == std::vector<Hotkey>{ 'O', VK_NUMPAD3 }
			|| key_command_box == std::vector<Hotkey>{ 'U' };
	}

	static void emplace_back_all(std::vector<Hotkey>& dest, const std::vector<Hotkey>& src)
	{
		for (const auto& elm : src)
		{
			dest.emplace_back(elm);
		}
	}

	std::vector<Hotkey> InputScheme::getAll() const
	{
		std::vector<Hotkey> all = {};
		emplace_back_all(all, key_menu_open_close);
		emplace_back_all(all, key_menu_root_up);
		emplace_back_all(all, key_menu_root_down);
		emplace_back_all(all, key_menu_up);
		emplace_back_all(all, key_menu_down);
		emplace_back_all(all, key_menu_left);
		emplace_back_all(all, key_menu_right);
		emplace_back_all(all, key_menu_click);
		emplace_back_all(all, key_menu_back);
		emplace_back_all(all, key_menu_context);
		emplace_back_all(all, key_command_box);
		return all;
	}

	bool InputScheme::conflictsWithGameplay() const
	{
		return conflictsWith(std::vector<Hotkey>{ VK_UP, VK_DOWN, VK_LEFT, VK_RIGHT });
	}

	bool InputScheme::conflictsWith(Hotkey hotkey) const
	{
		auto all = getAll();
		return hotkey.isAnyVkEqual(all);
	}

	bool InputScheme::conflictsWith(const std::vector<Hotkey>& hotkeys) const
	{
		for (const auto& hotkey : hotkeys)
		{
			if (conflictsWith(hotkey))
			{
				return true;
			}
		}
		return false;
	}

	uhash_t Input::getSchemeHash()
	{
		return keyboard_and_mouse ? soup::joaat::hashRange((const char*)&scheme, sizeof(scheme)) : 0;
	}

	bool Input::doesSchemeConflictWithInput(ControlInput control_input)
	{
		if (!keyboard_and_mouse)
		{
			return true;
		}
		const auto vk = ControlInput_to_VirtualKey(control_input);
		return vk == 0 || scheme.conflictsWith(vk);
	}

	std::string Input::vk_to_string_no_brackets(const unsigned int vk)
	{
		if ((vk >= 'A' && vk <= 'Z') || is_number_char(vk))
		{
			return std::string(1, vk);
		}
		for (const auto& entry : vk_string_map)
		{
			if (entry.key == vk)
			{
				return entry.to_string(*Lang::active_data);
			}
		}
		return LANG_GET("K_UNK");
	}

	std::string Input::vk_to_string(const unsigned int vk)
	{
		return vk_to_string(std::string(1, '['), vk, std::string(1, ']'));
	}

	std::string Input::vk_to_string(std::string&& prefix, const unsigned int vk, std::string&& suffix)
	{
		return prefix.append(vk_to_string_no_brackets(vk)).append(std::move(suffix));
	}

	std::string Input::vk_to_file_string(const unsigned int vk)
	{
		if ((vk >= 'A' && vk <= 'Z') || (vk >= '1' && vk <= '9'))
		{
			return std::string(1, vk);
		}
		auto lang = Lang::id_to_data(LANG_EN);
		for (const auto& entry : vk_string_map)
		{
			if (entry.key == vk)
			{
				return entry.to_string(*lang);
			}
		}
		return fmt::to_string(vk);
	}

	unsigned int Input::ControlInput_to_VirtualKey(ControlInput control_input)
	{
		if (NativeCallContext::canInvoke(0x0499D7B09FC9B407))
		{
			std::string instructional_button = PAD::GET_CONTROL_INSTRUCTIONAL_BUTTONS_STRING(0, control_input, true);
			if (instructional_button.substr(0, 2) == "b_")
			{
				switch (std::stoul(instructional_button.substr(2)))
				{
				case MOUSE_LEFT:
					return VK_LBUTTON;

				case MOUSE_RIGHT:
					return VK_RBUTTON;

				case MOUSE_MIDDLE:
					return VK_MBUTTON;

				case MOUSE_4:
					return VK_XBUTTON1;

				case MOUSE_5:
					return VK_XBUTTON2;

				case NUMPAD_MINUS:
					return VK_SUBTRACT;

				case NUMPAD_PLUS:
					return VK_ADD;

				case NUMPAD_PERIOD:
					return VK_OEM_PERIOD;

				case NUMPAD_SLASH:
				case NUMPAD_MULTIPLY:
					return VK_DIVIDE;

				case NUMPAD_STAR:
				case NUMPAD_DIVIDE:
					return VK_MULTIPLY;

				case NUMPAD_ENTER:
					return VK_RETURN;

				case NUMPAD_0:
					return VK_NUMPAD0;

				case NUMPAD_1:
					return VK_NUMPAD1;

				case NUMPAD_2:
					return VK_NUMPAD2;

				case NUMPAD_3:
					return VK_NUMPAD3;

				case NUMPAD_4:
					return VK_NUMPAD4;

				case NUMPAD_5:
					return VK_NUMPAD5;

				case NUMPAD_6:
					return VK_NUMPAD6;

				case NUMPAD_7:
					return VK_NUMPAD7;

				case NUMPAD_8:
					return VK_NUMPAD8;

				case NUMPAD_9:
					return VK_NUMPAD9;

				case KEY_BACKSPACE:
					return VK_BACK;

				case F1:
					return VK_F1;

				case F2:
					return VK_F2;

				case F3:
					return VK_F3;

				case F4:
					return VK_F4;

				case F5:
					return VK_F5;

				case F6:
					return VK_F6;

				case F7:
					return VK_F7;

				case F8:
					return VK_F8;

				case F9:
					return VK_F9;

				case F10:
					return VK_F10;

				case F11:
					return VK_F11;

				case F12:
					return VK_F12;

				case F13:
					return VK_F13;

				case F14:
					return VK_F14;

				case F15:
					return VK_F15;

				case F16:
					return VK_F16;

				case F17:
					return VK_F17;

				case F18:
					return VK_F18;

				case F19:
					return VK_F19;

				case F20:
					return VK_F20;

				case F21:
					return VK_F21;

				case F22:
					return VK_F22;

				case F23:
					return VK_F23;

				case F24:
					return VK_F24;

				case KEY_ARROW_UP:
					return VK_UP;

				case KEY_ARROW_DOWN:
					return VK_DOWN;

				case KEY_ARROW_LEFT:
					return VK_LEFT;

				case KEY_ARROW_RIGHT:
					return VK_RIGHT;

				case KEY_DEL:
				case KEY_ENTF:
					return VK_DELETE;

				case KEY_ESC:
					return VK_ESCAPE;

				case KEY_INS:
				case KEY_EINFG:
					return VK_INSERT;

				case KEY_END:
				case KEY_ENDE:
					return VK_END;
				}
			}
			else if (instructional_button.substr(0, 2) == "t_")
			{
				const char c = instructional_button.at(2);
				if ((c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'))
				{
					return c;
				}
			}
		}
		return 0;
	}

	Hotkey Input::getPreferedHotkey(const std::vector<Hotkey>& hotkeys)
	{
		for (const auto& hotkey : hotkeys)
		{
			if (hotkey.isNumpadKey() == Input::user_has_numpad)
			{
				return hotkey;
			}
		}
		return hotkeys.at(0);
	}

	void Input::addToScaleform(ControlInput control_input)
	{
		GRAPHICS::SCALEFORM_MOVIE_METHOD_ADD_PARAM_PLAYER_NAME_STRING(PAD::GET_CONTROL_INSTRUCTIONAL_BUTTONS_STRING(0, control_input, true));
	}

	void Input::addToScaleform(const std::vector<Hotkey>& hotkeys, ControlInput controller_input)
	{
		if (!keyboard_and_mouse)
		{
			addToScaleform(controller_input);
			return;
		}
		auto hotkey = getPreferedHotkey(hotkeys);
		if (!hotkey.hasModifiers())
		{
			if (hotkey.vk() >= VK_NUMPAD0 && hotkey.vk() <= VK_NUMPAD9)
			{
				GRAPHICS::SCALEFORM_MOVIE_METHOD_ADD_PARAM_INT(hotkey.vk() + 40);
				return;
			}
		}
		std::string str = hotkey.toString();
		if (str.size() > 2)
		{
			GRAPHICS::SCALEFORM_MOVIE_METHOD_ADD_PARAM_PLAYER_NAME_STRING(str.insert(0, "w_").c_str());
		}
		else
		{
			GRAPHICS::SCALEFORM_MOVIE_METHOD_ADD_PARAM_PLAYER_NAME_STRING(str.insert(0, "t_").c_str());
		}
	}

	bool Input::isController() noexcept
	{
		return !keyboard_and_mouse;
	}

	bool Input::isMenuClosedOrControllerUnused() noexcept
	{
		return !isController() || !g_gui.opened;
	}

	bool Input::isContextAvailable() noexcept
	{
		return isMenuClosedOrControllerUnused();
	}

	bool Input::isFreeSprintAvailable() noexcept
	{
		return !isController() || !g_gui.opened || !g_renderer.areTabsEnabled();
	}

	bool Input::isAscDescAvailable() noexcept
	{
		return isMenuClosedOrControllerUnused();
	}

	ControlInput Input::getContext() noexcept
	{
		return keyboard_and_mouse ? INPUT_CONTEXT : INPUT_FRONTEND_ACCEPT;
	}

	ControlInput Input::getFreeSprint() noexcept
	{
#if CONTROLLER_ANALOG_ASCDESC
		return keyboard_and_mouse ? INPUT_JUMP : INPUT_FRONTEND_ACCEPT;
#else
		return keyboard_and_mouse ? INPUT_JUMP : INPUT_FRONTEND_LT;
#endif
	}

	ControlInput Input::getAscend() noexcept
	{
#if CONTROLLER_ANALOG_ASCDESC
		return keyboard_and_mouse ? INPUT_JUMP : INPUT_FRONTEND_ACCEPT;
#else
		return keyboard_and_mouse ? INPUT_SPRINT : INPUT_FRONTEND_ACCEPT;
#endif
	}

	ControlInput Input::getDescend() noexcept
	{
#if CONTROLLER_ANALOG_ASCDESC
		return keyboard_and_mouse ? INPUT_JUMP : INPUT_FRONTEND_ACCEPT;
#else
		return keyboard_and_mouse ? INPUT_DUCK : INPUT_FRONTEND_X;
#endif
	}

	bool Input::isCommandInputAllowed()
	{
		return g_renderer.gameplayState == PLAYING || g_renderer.gameplayState == MENUING;
	}

	bool Input::canMovementCommandPerformMovement()
	{
		return g_tb_freecam.canMovementCommandPerformMovement()
			&& !GhostDriver::user_is_rcing
			;
	}

	bool Input::isPressingAim()
	{
		if (g_player_veh.isValid())
		{
			return PAD::IS_DISABLED_CONTROL_PRESSED(0, INPUT_VEH_AIM);
		}
		return PAD::IS_DISABLED_CONTROL_PRESSED(0, INPUT_AIM);
	}

	bool Input::isPressingAttack()
	{
		if (g_player_veh.isValid())
		{
			if (VEHICLE::DOES_VEHICLE_HAVE_WEAPONS(g_player_veh))
			{
				if (PAD::IS_DISABLED_CONTROL_PRESSED(0, INPUT_VEH_ATTACK2))
				{
					return true;
				}
				if (VEHICLE::IS_THIS_MODEL_A_PLANE(g_player_veh.getModel()))
				{
					return PAD::IS_DISABLED_CONTROL_PRESSED(0, INPUT_VEH_FLY_ATTACK);
				}
				return false;
			}
			return PAD::IS_DISABLED_CONTROL_PRESSED(0, INPUT_VEH_ATTACK);
		}
		return PAD::IS_DISABLED_CONTROL_PRESSED(0, INPUT_ATTACK);
	}

	float Input::getControlNormal(const int control_input)
	{
		return isCommandInputAllowed() ? PAD::GET_DISABLED_CONTROL_NORMAL(0, control_input) : 0.0f;
	}

	float Input::getControlNormal(const int positive, const int negative)
	{
		return getControlNormal(positive) - getControlNormal(negative);
	}

	bool Input::isControlPressed(const int control_input)
	{
		return isCommandInputAllowed() && PAD::IS_DISABLED_CONTROL_PRESSED(0, control_input);;
	}

	bool Input::isControlJustPressed(const int control_input)
	{
		return isCommandInputAllowed() && PAD::IS_DISABLED_CONTROL_JUST_PRESSED(0, control_input);;
	}

	bool Input::isContextJustPressed()
	{
		return isContextJustPressed(INPUT_FRONTEND_ACCEPT);
	}

	bool Input::isContextJustPressed(const int input_for_controller)
	{
		if (Input::isController())
		{
			return isControlJustPressed(input_for_controller);
		}
		PAD::DISABLE_CONTROL_ACTION(0, INPUT_CONTEXT, TRUE);
		PAD::DISABLE_CONTROL_ACTION(0, INPUT_VEH_ROCKET_BOOST, TRUE);
		return isControlJustPressed(INPUT_CONTEXT);
	}

	bool Input::isAnyKeyPressed(const std::vector<Hotkey>& hotkeys, const int control_input)
	{
		if (!g_tb_freecam.isEnabled() && isCommandInputAllowed())
		{
			if (!Input::keyboard_and_mouse)
			{
				return PAD::IS_DISABLED_CONTROL_PRESSED(0, control_input);
			}
			if (GetForegroundWindow() == pointers::hwnd)
			{
				for (const auto& hotkey : hotkeys)
				{
					if (hotkey.isPressedAsync())
					{
						return true;
					}
				}
			}
		}
		return false;
	}

	float Input::getHighestNormal(const std::vector<Hotkey>& hotkeys, const int control_input)
	{
		if (!g_tb_freecam.isEnabled() && isCommandInputAllowed())
		{
			if (!Input::keyboard_and_mouse)
			{
				return PAD::GET_DISABLED_CONTROL_NORMAL(0, control_input);
			}
			float highest = 0.0f;
			if (GetForegroundWindow() == pointers::hwnd)
			{
				for (const auto& hotkey : hotkeys)
				{
					highest = std::max(highest, hotkey.getNormal());
				}
			}
			return highest;
		}
		return 0.0f;
	}
}
