#pragma once

namespace Stand
{
	template <typename T>
	[[nodiscard]] constexpr bool is_number_char(const T c) noexcept
	{
		return (c >= '0' && c <= '9');
	}
}
