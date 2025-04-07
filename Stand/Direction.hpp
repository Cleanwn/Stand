#pragma once

#include <cstdint>

namespace Stand
{
	enum Direction : uint8_t
	{
		NONE = 0,
		TELEPORT = NONE,
		UP,
		TOP = UP,
		DOWN,
		BOTTOM = DOWN,
		FORWARD,
		BACK,
		LEFT,
		RIGHT,
	};
}
