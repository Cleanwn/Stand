#pragma once

#include <cstdint>
#include <string>

namespace Stand
{
#pragma pack(push, 1)
	struct TimeOfDay
	{
		int hour = 0;
		int minute = 0;
		int second = 0;

		[[nodiscard]] static TimeOfDay current() noexcept;

		[[nodiscard]] bool isValid() const noexcept;
		void invalidate() noexcept;

		[[nodiscard]] int asSeconds() const noexcept;

		[[nodiscard]] std::string toString() const noexcept;

		[[nodiscard]] static TimeOfDay fromString(const std::string& str);
		[[nodiscard]] static TimeOfDay fromString(const std::wstring& str);
	};
#pragma pack(pop)
}
