#pragma once

#include <cstdint>

namespace Stand
{
	struct Position2d
	{
		int16_t x = SHRT_MAX;
		int16_t y = SHRT_MAX;

		[[nodiscard]] bool isValid() const noexcept
		{
			return x != SHRT_MAX && y != SHRT_MAX;
		}

		void invalidate()
		{
			x = SHRT_MAX;
			y = SHRT_MAX;
		}

		[[nodiscard]] bool operator ==(const Position2d& b) const
		{
			return x == b.x && y == b.y;
		}

		[[nodiscard]] bool operator !=(const Position2d& b) const
		{
			return !operator ==(b);
		}
	};
}
