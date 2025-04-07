#include "Hotkey.hpp"

#include <soup/Key.hpp>

#include "input.hpp"
#include "is_number_char.hpp"
#include "lang.hpp"
#include "LangId.hpp"
#include "str2int.hpp"
#include "StringUtils.hpp"
#include "vk_string_map.hpp"

namespace Stand
{
	Hotkey::Hotkey(std::string str)
		: value(0)
	{
		StringUtils::to_lower(str);
		if (str.length() > 5 && str.substr(0, 5) == "hold ")
		{
			str.erase(0, 5);
			value |= FLAG_HOLDMODE;
		}
		if (str.length() > 5 && str.substr(0, 5) == "ctrl+")
		{
			str.erase(0, 5);
			value |= FLAG_CTRL;
		}
		if (str.length() > 6 && str.substr(0, 6) == "shift+")
		{
			str.erase(0, 6);
			value |= FLAG_SHIFT;
		}
		if (str.length() > 4 && str.substr(0, 4) == "alt+")
		{
			str.erase(0, 4);
			value |= FLAG_ALT;
		}
		if (str.length() == 1)
		{
			const char c = str.at(0);
			if (is_number_char(c))
			{
				value |= c;
				return;
			}
			if (c >= 'a' && c <= 'z')
			{
				value |= (c + ('A' - 'a'));
				return;
			}
		}
		for (const auto& entry : vk_string_map)
		{
			auto lang = Lang::id_to_data(LANG_EN);
			auto entry_str = entry.to_string(*lang);
			StringUtils::to_lower(entry_str);
			if (entry_str == str)
			{
				value |= entry.key;
				return;
			}
		}
		auto res = str2int<unsigned int>(str);
		if (res.has_value())
		{
			value |= res.value();
			return;
		}
		value = 0;
	}

	Hotkey Hotkey::fromPressing(unsigned int vk, bool repeat)
	{
		bool ctrl = false, shift = false, alt = false;
		if (vk != VK_CONTROL && vk != VK_LCONTROL && vk != VK_RCONTROL)
		{
			ctrl = (GetKeyState(VK_CONTROL) & 0x8000);
		}
		if (vk != VK_SHIFT && vk != VK_LSHIFT && vk != VK_RSHIFT)
		{
			shift = (GetKeyState(VK_SHIFT) & 0x8000);
		}
		if (vk != VK_MENU && vk != VK_LMENU && vk != VK_RMENU)
		{
			alt = (GetKeyState(VK_MENU) & 0x8000);
		}
		return Hotkey(vk, ctrl, shift, alt, repeat);
	}

	void Hotkey::operator=(const Hotkey b)
	{
		value = b.value;
	}

	bool Hotkey::operator==(const Hotkey b) const
	{
		return getKeyValue() == b.getKeyValue();
	}

	bool Hotkey::operator!=(const Hotkey b) const
	{
		return !operator==(b);
	}

	Hotkey::operator bool() const
	{
		return value != 0;
	}

	void Hotkey::reset()
	{
		value = 0;
	}

	unsigned int Hotkey::vk() const
	{
		return (unsigned int)(value & 0xFFFFFFFFF); // 32 bits
	}

	bool Hotkey::hasModkeys() const
	{
		return (value >> 32) & 0b111;
	}

	bool Hotkey::hasModifiers() const
	{
		return value >> 32;
	}

	bool Hotkey::ctrl() const
	{
		return value & FLAG_CTRL;
	}

	bool Hotkey::shift() const
	{
		return value & FLAG_SHIFT;
	}

	bool Hotkey::alt() const
	{
		return value & FLAG_ALT;
	}

	uint64_t Hotkey::getKeyValue() const
	{
		return value & 0b11111111111111111111111111111111111; // 35 bits
	}

	bool Hotkey::isHoldMode() const
	{
		return value & FLAG_HOLDMODE;
	}

	void Hotkey::setHoldMode(bool on) noexcept
	{
		if (on)
		{
			enableHoldMode();
		}
		else
		{
			value &= ~FLAG_HOLDMODE;
		}
	}

	void Hotkey::enableHoldMode() noexcept
	{
		value |= FLAG_HOLDMODE;
	}

	bool Hotkey::isRepeat() const
	{
		return value & FLAG_REPEAT;
	}

	std::string Hotkey::toString() const
	{
		std::string str = {};
		if (isHoldMode())
		{
			str.append(LANG_GET("K_HOLD"));
			str.push_back(' ');
		}
		if (ctrl())
		{
			str.append(LANG_GET("K_CTRL"));
			str.push_back('+');
		}
		if (shift())
		{
			str.append(LANG_GET("K_SHFT"));
			str.push_back('+');
		}
		if (alt())
		{
			str.append(LANG_GET("K_ALT"));
			str.push_back('+');
		}
		return str.append(Input::vk_to_string_no_brackets(vk()));
	}

	std::wstring Hotkey::toWString() const
	{
		return StringUtils::utf8_to_utf16(toString());
	}

	CommandName Hotkey::toStringForCommandName() const
	{
#if COMPACT_COMMAND_NAMES
		return toString();
#else
		return toWString();
#endif
	}

	std::string Hotkey::toBracketedString() const
	{
		auto str = std::string(1, '[');
		str.append(toString());
		str.push_back(']');
		return str;
	}

	std::string Hotkey::toFileString() const
	{
		std::string str = {};
		if (isHoldMode())
		{
			str.append("Hold ");
		}
		if (ctrl())
		{
			str.append("Ctrl+");
		}
		if (shift())
		{
			str.append("Shift+");
		}
		if (alt())
		{
			str.append("Alt+");
		}
		return str.append(Input::vk_to_file_string(vk()));
	}

	bool Hotkey::isPressedAsync() const
	{
		const auto vk = this->vk();
		if (!(GetAsyncKeyState(vk) & 0x8000))
		{
			return false;
		}
		return fromPressing(vk).overlapsWith(*this);
	}

	// rage_input_helper.asm
	extern "C" float(*rage_input_helper_get_override())[256];

	float Hotkey::getNormal() const
	{
		const auto vk = this->vk();
		if (fromPressing(vk).overlapsWith(*this)) // Modifier keys match?
		{
			// Check analogue input
			if (float(*arr)[256] = rage_input_helper_get_override())
			{
				if (uint8_t sk = soup::virtual_key_to_soup_key(vk); sk != soup::KEY_NONE) // with respect to current layout
				{
					return (*arr)[soup::soup_key_to_virtual_key(sk)]; // without respect to current layout
				}
			}

			// Check digital input
			if (GetAsyncKeyState(vk) & 0x8000)
			{
				return 1.0f;
			}
		}
		return 0.0f;
	}

	bool Hotkey::overlapsWith(const Hotkey b) const
	{
		if (vk() != b.vk())
		{
			return false;
		}
		HotkeyModkeyBehaviour current_modkey_behaviour = Hotkey::modkey_behaviour;
		if (current_modkey_behaviour == HMB_STRICT_IF_HAS_MODKEY)
		{
			current_modkey_behaviour = b.hasModkeys() ? HMB_STRICT : HMB_LAX;
		}
		if (current_modkey_behaviour == HMB_STRICT)
		{
			return ctrl() == b.ctrl()
				&& shift() == b.shift()
				&& alt() == b.alt();
		}
		else // if (current_modkey_behaviour == HMB_LAX)
		{
			if (b.ctrl() && !ctrl())
			{
				return false;
			}
			if (b.shift() && !shift())
			{
				return false;
			}
			if (b.alt() && !alt())
			{
				return false;
			}
			return true;
		}
	}

	const Hotkey* Hotkey::getOverlap(const std::vector<Hotkey>& hotkeys) const
	{
		for (const auto& hotkey : hotkeys)
		{
			if (overlapsWith(hotkey))
			{
				return &hotkey;
			}
		}
		return nullptr;
	}

	bool Hotkey::overlapsWith(const std::vector<Hotkey>& hotkeys) const
	{
		for (const auto& hotkey : hotkeys)
		{
			if (overlapsWith(hotkey))
			{
				return true;
			}
		}
		return false;
	}

	bool Hotkey::overlapsWithIncludeHoldMode(const Hotkey b) const
	{
		return overlapsWith(b)
			&& isHoldMode() == b.isHoldMode();
	}

	bool Hotkey::overlapsWithIncludeHoldMode(const std::vector<Hotkey>& hotkeys) const
	{
		for (const auto& hotkey : hotkeys)
		{
			if (overlapsWithIncludeHoldMode(hotkey))
			{
				return true;
			}
		}
		return false;
	}

	bool Hotkey::isAnyVkEqual(const std::vector<Hotkey>& hotkeys) const
	{
		for (const auto& hotkey : hotkeys)
		{
			if (vk() == hotkey.vk())
			{
				return true;
			}
		}
		return false;
	}

	bool Hotkey::isNumpadKey() const
	{
		switch (vk())
		{
		case VK_NUMPAD0:
		case VK_NUMPAD1:
		case VK_NUMPAD2:
		case VK_NUMPAD3:
		case VK_NUMPAD4:
		case VK_NUMPAD5:
		case VK_NUMPAD6:
		case VK_NUMPAD7:
		case VK_NUMPAD8:
		case VK_NUMPAD9:
		case VK_ADD:
		case VK_SUBTRACT:
		case VK_MULTIPLY:
		case VK_DIVIDE:
			return true;
		}
		return false;
	}
}
