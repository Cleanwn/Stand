#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>

#include <fmt/core.h>

#include "atStringHash.hpp"
#include "lang.hpp"
#include "LangData.hpp"

namespace Stand
{
	enum KeyTranslateInfo : uint8_t
	{
		LITERAL,
		NORMAL,
		KT_RIGHT,
		KT_LEFT,
		NUMPAD_LITERAL,
		NUMPAD_TRANS,
	};

	struct KeyData
	{
		const unsigned int key;
		const char* const name;
		const KeyTranslateInfo translate_info = NORMAL;

		constexpr KeyData(unsigned int key, const void* name, KeyTranslateInfo translate_info = NORMAL)
			: key(key), name(static_cast<const char*>(name)), translate_info(translate_info)
		{
		}

		[[nodiscard]] std::string to_string(const LangData& lang) const
		{
			switch (translate_info)
			{
			case LITERAL:
			default:
				return name;

			case NORMAL:
				return lang.get(name);

			case KT_RIGHT:
				return fmt::format(fmt::runtime(lang.get(ATSTRINGHASH("K_R"))), lang.get(name));

			case KT_LEFT:
				return fmt::format(fmt::runtime(lang.get(ATSTRINGHASH("K_L"))), lang.get(name));

			case NUMPAD_LITERAL:
			{
				std::string str = lang.get(ATSTRINGHASH("KSCHM_1"));
				str.push_back(' ');
				str.append(name);
				return str;
			}

			case NUMPAD_TRANS:
			{
				std::string str = lang.get(ATSTRINGHASH("KSCHM_1"));
				str.push_back(' ');
				str.append(lang.get(name));
				return str;
			}

			}
		}
	};

	inline KeyData vk_string_map[] = {
		KeyData(VK_F1, "F1", LITERAL),
		KeyData(VK_F2, "F2", LITERAL),
		KeyData(VK_F3, "F3", LITERAL),
		KeyData(VK_F4, "F4", LITERAL),
		KeyData(VK_F5, "F5", LITERAL),
		KeyData(VK_F6, "F6", LITERAL),
		KeyData(VK_F7, "F7", LITERAL),
		KeyData(VK_F8, "F8", LITERAL),
		KeyData(VK_F9, "F9", LITERAL),
		KeyData(VK_F10, "F10", LITERAL),
		KeyData(VK_F11, "F11", LITERAL),
		KeyData(VK_F12, "F12", LITERAL),
		KeyData(VK_F13, "F13", LITERAL),
		KeyData(VK_F14, "F14", LITERAL),
		KeyData(VK_F15, "F15", LITERAL),
		KeyData(VK_F16, "F16", LITERAL),
		KeyData(VK_F17, "F17", LITERAL),
		KeyData(VK_F18, "F18", LITERAL),
		KeyData(VK_F19, "F19", LITERAL),
		KeyData(VK_F20, "F20", LITERAL),
		KeyData(VK_F21, "F21", LITERAL),
		KeyData(VK_F22, "F22", LITERAL),
		KeyData(VK_F23, "F23", LITERAL),
		KeyData(VK_F24, "F24", LITERAL),
		KeyData(220, "^", LITERAL),
		KeyData(VK_OEM_2, "#", LITERAL),
		KeyData(VK_OEM_4, "[", LITERAL),
		KeyData(VK_OEM_5, "\\", LITERAL),
		KeyData(VK_OEM_6, "]", LITERAL),
		KeyData(VK_OEM_7, "'", LITERAL),
		KeyData(VK_OEM_102, "<>", LITERAL),
		KeyData(VK_NUMPAD0, "0", NUMPAD_LITERAL),
		KeyData(VK_NUMPAD1, "1", NUMPAD_LITERAL),
		KeyData(VK_NUMPAD2, "2", NUMPAD_LITERAL),
		KeyData(VK_NUMPAD3, "3", NUMPAD_LITERAL),
		KeyData(VK_NUMPAD4, "4", NUMPAD_LITERAL),
		KeyData(VK_NUMPAD5, "5", NUMPAD_LITERAL),
		KeyData(VK_NUMPAD6, "6", NUMPAD_LITERAL),
		KeyData(VK_NUMPAD7, "7", NUMPAD_LITERAL),
		KeyData(VK_NUMPAD8, "8", NUMPAD_LITERAL),
		KeyData(VK_NUMPAD9, "9", NUMPAD_LITERAL),
		KeyData(VK_DECIMAL, ".", NUMPAD_LITERAL),
		KeyData(VK_ADD, "+", NUMPAD_LITERAL),
		KeyData(VK_SUBTRACT, "-", NUMPAD_LITERAL),
		KeyData(VK_MULTIPLY, "*", NUMPAD_LITERAL),
		KeyData(VK_OEM_MINUS, "/", NUMPAD_LITERAL),
		KeyData(VK_SHIFT, "K_SHFT"),
		KeyData(VK_RSHIFT, "K_SHFT", KT_RIGHT),
		KeyData(VK_LSHIFT, "K_SHFT", KT_LEFT),
		KeyData(VK_MENU, "K_ALT"),
		KeyData(VK_RMENU, "K_ALT", KT_RIGHT),
		KeyData(VK_LMENU, "K_ALT", KT_LEFT),
		KeyData(VK_CONTROL, "K_CTRL"),
		KeyData(VK_RCONTROL, "K_CTRL", KT_RIGHT),
		KeyData(VK_LCONTROL, "K_CTRL", KT_LEFT),
		KeyData(VK_SPACE, "K_SPC"),
		KeyData(VK_INSERT, "K_ISRT"),
		KeyData(VK_DELETE, "K_DEL"),
		KeyData(VK_HOME, "K_HME"),
		KeyData(VK_END, "K_END"),
		KeyData(VK_PRIOR, "K_PGUP"),
		KeyData(VK_NEXT, "K_PGDN"),
		KeyData(VK_RETURN, "K_RET"),
		KeyData(0x0E, "K_RET", NUMPAD_TRANS),
		KeyData(VK_BACK, "K_BACK"),
		KeyData(VK_ESCAPE, "K_ESC"),
		KeyData(VK_UP, "K_A_U"),
		KeyData(VK_LEFT, "K_A_L"),
		KeyData(VK_RIGHT, "K_A_R"),
		KeyData(VK_DOWN, "K_A_D"),
		KeyData(VK_TAB, "K_TAB"),
		KeyData(222, u8"Ä", LITERAL),
		KeyData(192, u8"Ö", LITERAL),
		KeyData(186, u8"Ü", LITERAL),
		KeyData(219, u8"ß", LITERAL),
		KeyData(VK_LBUTTON, "K_LMB"),
		KeyData(VK_MBUTTON, "K_MMB"),
		KeyData(VK_RBUTTON, "K_RMB"),
		KeyData(VK_XBUTTON1, "K_X1"),
		KeyData(VK_XBUTTON2, "K_X2"),
		KeyData(VK_OEM_PLUS, "+", LITERAL),
		KeyData(VK_OEM_PERIOD, ".", LITERAL),
		KeyData(VK_OEM_COMMA, ",", LITERAL),
		KeyData(VK_MEDIA_PLAY_PAUSE, "K_MPP"),
		KeyData(VK_MEDIA_STOP, "K_MS"),
		KeyData(VK_MEDIA_NEXT_TRACK, "K_NT"),
		KeyData(VK_MEDIA_PREV_TRACK, "K_PT"),
		KeyData(VK_CAPITAL, "K_CAPS"),
	};
}
