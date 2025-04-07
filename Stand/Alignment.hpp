#pragma once

#include <cstdint>

namespace Stand
{
	enum Alignment : uint8_t
	{
		ALIGN_TOP_LEFT      = 0b0000,
		ALIGN_TOP_CENTRE    = 0b0001,
		ALIGN_TOP_RIGHT     = 0b0010,
		ALIGN_CENTRE_LEFT   = 0b0100,
		ALIGN_CENTRE        = 0b0101,
		ALIGN_CENTRE_RIGHT  = 0b0110,
		ALIGN_BOTTOM_LEFT   = 0b1000,
		ALIGN_BOTTOM_CENTRE = 0b1001,
		ALIGN_BOTTOM_RIGHT  = 0b1010,
	};
}
