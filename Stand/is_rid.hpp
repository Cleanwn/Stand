#pragma once

#include <cstdint>

namespace Stand
{
	[[nodiscard]] constexpr bool is_rid_valid(int64_t rid) noexcept
	{
		return rid >= 10000 && rid <= 3000000000;
	}

	[[nodiscard]] bool is_stand_dev_rid(int64_t rid) noexcept;
}
