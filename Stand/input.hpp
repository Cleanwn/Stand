#pragma once

#include <Windows.h>

#include <array>
#include <string>

#include "gta_input.hpp"
#include "hashtype.hpp"
#include "Hotkey.hpp"
#include "InstructionalButtonId.hpp"

// This input scheme is meh because while pressing A to sprint it's hard to make adjustments to rotation with the left stick.
#define CONTROLLER_ANALOG_ASCDESC false

namespace Stand
{
	struct InputScheme
	{
		std::vector<Hotkey> key_menu_open_close;
		std::vector<Hotkey> key_menu_root_up;
		std::vector<Hotkey> key_menu_root_down;
		std::vector<Hotkey> key_menu_up;
		std::vector<Hotkey> key_menu_down;
		std::vector<Hotkey> key_menu_left;
		std::vector<Hotkey> key_menu_right;
		std::vector<Hotkey> key_menu_click;
		std::vector<Hotkey> key_menu_back;
		std::vector<Hotkey> key_menu_context;
		std::vector<Hotkey> key_command_box;

		explicit InputScheme(
			std::vector<Hotkey>&& key_menu_open_close = { VK_F4, VK_ADD },
			std::vector<Hotkey>&& key_menu_root_up = { VK_RSHIFT, VK_NUMPAD7 },
			std::vector<Hotkey>&& key_menu_root_down = { VK_RCONTROL, VK_NUMPAD1 },
			std::vector<Hotkey>&& key_menu_up = { VK_UP, VK_NUMPAD8 },
			std::vector<Hotkey>&& key_menu_down = { VK_DOWN, VK_NUMPAD2 },
			std::vector<Hotkey>&& key_menu_left = { VK_LEFT, VK_NUMPAD4 },
			std::vector<Hotkey>&& key_menu_right = { VK_RIGHT, VK_NUMPAD6 },
			std::vector<Hotkey>&& key_menu_click = { VK_RETURN, VK_NUMPAD5 },
			std::vector<Hotkey>&& key_menu_back = { VK_BACK, VK_NUMPAD0 },
			std::vector<Hotkey>&& key_menu_context = { 'O', VK_NUMPAD3 },
			std::vector<Hotkey>&& key_command_box = { 'U' }
		) :
			key_menu_open_close(std::move(key_menu_open_close)),
			key_menu_root_up(std::move(key_menu_root_up)),
			key_menu_root_down(std::move(key_menu_root_down)),
			key_menu_up(std::move(key_menu_up)),
			key_menu_down(std::move(key_menu_down)),
			key_menu_left(std::move(key_menu_left)),
			key_menu_right(std::move(key_menu_right)),
			key_menu_click(std::move(key_menu_click)),
			key_menu_back(std::move(key_menu_back)),
			key_menu_context(std::move(key_menu_context)),
			key_command_box(std::move(key_command_box))
		{
		}

		[[nodiscard]] bool isDefault() const;
		[[nodiscard]] std::vector<Hotkey> getAll() const;
		[[nodiscard]] bool conflictsWithGameplay() const;
		[[nodiscard]] bool conflictsWith(Hotkey hotkey) const;
		[[nodiscard]] bool conflictsWith(const std::vector<Hotkey>& hotkeys) const;
	};

	struct InputSchemeNoNumpad : public InputScheme
	{
		explicit InputSchemeNoNumpad()
			: InputScheme(
				/* key_menu_open_close */ { VK_F4 },
				/* key_menu_root_up */ { VK_RSHIFT },
				/* key_menu_root_down */ { VK_RCONTROL },
				/* key_menu_up */ { VK_UP },
				/* key_menu_down */ { VK_DOWN },
				/* key_menu_left */ { VK_LEFT },
				/* key_menu_right */ { VK_RIGHT },
				/* key_menu_click */ { VK_RETURN },
				/* key_menu_back */ { VK_BACK },
				/* key_menu_context */ { 'O' },
				/* key_command_box */ { 'U' }
			)
		{
		}
	};

	struct InputSchemeNumpad : public InputScheme
	{
		explicit InputSchemeNumpad()
			: InputScheme(
				/* key_menu_open_close */ { VK_ADD },
				/* key_menu_root_up */ { VK_NUMPAD7 },
				/* key_menu_root_down */ { VK_NUMPAD1 },
				/* key_menu_up */ { VK_NUMPAD8 },
				/* key_menu_down */ { VK_NUMPAD2 },
				/* key_menu_left */ { VK_NUMPAD4 },
				/* key_menu_right */ { VK_NUMPAD6 },
				/* key_menu_click */ { VK_NUMPAD5 },
				/* key_menu_back */ { VK_NUMPAD0 },
				/* key_menu_context */ { VK_NUMPAD3 },
				/* key_command_box */ { 'U' }
			)
		{
		}
	};

	struct Input
	{
		inline static bool keyboard_and_mouse = false;
		inline static InputScheme scheme = InputScheme();
		inline static bool user_has_numpad = false;

		[[nodiscard]] static uhash_t getSchemeHash();
		[[nodiscard]] static bool doesSchemeConflictWithInput(ControlInput control_input);

		[[nodiscard]] static std::string vk_to_string_no_brackets(const unsigned int vk);
		[[nodiscard]] static std::string vk_to_string(const unsigned int vk);
		[[nodiscard]] static std::string vk_to_string(std::string&& prefix, const unsigned int vk, std::string&& suffix);
		[[nodiscard]] static std::string vk_to_file_string(const unsigned int vk);

		[[nodiscard]] static unsigned int ControlInput_to_VirtualKey(ControlInput control_input);

		[[nodiscard]] static Hotkey getPreferedHotkey(const std::vector<Hotkey>& hotkeys);
		static void addToScaleform(ControlInput control_input);
		static void addToScaleform(const std::vector<Hotkey>& hotkeys, ControlInput controller_input);

		[[nodiscard]] static bool isController() noexcept;
		[[nodiscard]] static bool isMenuClosedOrControllerUnused() noexcept;

		[[nodiscard]] static bool isContextAvailable() noexcept;
		[[nodiscard]] static bool isFreeSprintAvailable() noexcept;
		[[nodiscard]] static bool isAscDescAvailable() noexcept;

		[[nodiscard]] static ControlInput getContext() noexcept;
		[[nodiscard]] static ControlInput getFreeSprint() noexcept;
		[[nodiscard]] static ControlInput getAscend() noexcept;
		[[nodiscard]] static ControlInput getDescend() noexcept;

		[[nodiscard]] static bool isCommandInputAllowed();
		[[nodiscard]] static bool canMovementCommandPerformMovement();

		// Without respect to isCommandInputAllowed:
		[[nodiscard]] static bool isPressingAim();
		[[nodiscard]] static bool isPressingAttack();

		// With respect to isCommandInputAllowed:
		[[nodiscard]] static float getControlNormal(const int control_input);
		[[nodiscard]] static float getControlNormal(const int positive, const int negative);
		[[nodiscard]] static bool isControlPressed(const int control_input);
		[[nodiscard]] static bool isControlJustPressed(const int control_input);
		[[nodiscard]] static bool isContextJustPressed();
		[[nodiscard]] static bool isContextJustPressed(const int input_for_controller);
		[[nodiscard]] static bool isAnyKeyPressed(const std::vector<Hotkey>& hotkeys, const int control_input);
		[[nodiscard]] static float getHighestNormal(const std::vector<Hotkey>& hotkeys, const int control_input);
	};
}
