#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "CommandName.hpp"

// 32 bits are reserved for the virtual key value
#define FLAG_CTRL		0b100000000000000000000000000000000
#define FLAG_SHIFT		0b1000000000000000000000000000000000
#define FLAG_ALT		0b10000000000000000000000000000000000
#define FLAG_HOLDMODE	0b100000000000000000000000000000000000
#define FLAG_REPEAT		0b1000000000000000000000000000000000000

namespace Stand
{
	enum HotkeyModkeyBehaviour : uint8_t
	{
		HMB_STRICT,
		HMB_STRICT_IF_HAS_MODKEY,
		HMB_LAX,
	};

	class Hotkey
	{
	private:
		uint64_t value;

	public:
		inline static HotkeyModkeyBehaviour modkey_behaviour = HMB_STRICT_IF_HAS_MODKEY;

		constexpr Hotkey(uint64_t value = 0)
			: value(value)
		{
		}

		explicit constexpr Hotkey(unsigned int vk, bool ctrl, bool shift, bool alt, bool repeat = false)
			: value(vk)
		{
			if (ctrl)
			{
				value |= FLAG_CTRL;
			}
			if (shift)
			{
				value |= FLAG_SHIFT;
			}
			if (alt)
			{
				value |= FLAG_ALT;
			}
			if (repeat)
			{
				value |= FLAG_REPEAT;
			}
		}

		explicit Hotkey(std::string str);
		
		[[nodiscard]] static Hotkey fromPressing(unsigned int vk, bool repeat = false);

		void operator=(const Hotkey b);
		bool operator==(const Hotkey b) const;
		bool operator!=(const Hotkey b) const;
		explicit operator bool() const;

		void reset();

		[[nodiscard]] unsigned int vk() const;
		[[nodiscard]] bool hasModkeys() const;
		[[nodiscard]] bool hasModifiers() const;
		[[nodiscard]] bool ctrl() const;
		[[nodiscard]] bool shift() const;
		[[nodiscard]] bool alt() const;
		[[nodiscard]] uint64_t getKeyValue() const;
		
		[[nodiscard]] bool isHoldMode() const;
		void setHoldMode(bool on) noexcept;
		void enableHoldMode() noexcept;

		[[nodiscard]] bool isRepeat() const;

		[[nodiscard]] std::string toString() const;
		[[nodiscard]] std::wstring toWString() const;
		[[nodiscard]] CommandName toStringForCommandName() const;
		[[nodiscard]] std::string toBracketedString() const;
		[[nodiscard]] std::string toFileString() const;
		[[nodiscard]] bool isPressedAsync() const;
		[[nodiscard]] float getNormal() const;
		[[nodiscard]] bool overlapsWith(const Hotkey b) const;
		[[nodiscard]] const Hotkey* getOverlap(const std::vector<Hotkey>& hotkeys) const;
		[[nodiscard]] bool overlapsWith(const std::vector<Hotkey>& hotkeys) const;
		[[nodiscard]] bool overlapsWithIncludeHoldMode(const Hotkey b) const;
		[[nodiscard]] bool overlapsWithIncludeHoldMode(const std::vector<Hotkey>& hotkeys) const;
		[[nodiscard]] bool isAnyVkEqual(const std::vector<Hotkey>& hotkeys) const;
		[[nodiscard]] bool isNumpadKey() const;
	};
	static_assert(sizeof(Hotkey) <= 8, "Hotkey is bigger than a pointer/reference");
}
