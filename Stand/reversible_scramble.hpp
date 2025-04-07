#pragma once

#include <cstdint>

namespace Stand
{
	[[nodiscard]] __forceinline uint64_t reversible_scramble(uint64_t val) noexcept
	{
		for (uint8_t i = 0; i < 8; ++i)
		{
			auto byte = &((uint8_t*)&val)[i];

			auto hi_nibble = *byte & 0xF0;
			auto lo_nibble = *byte & 0x0F;

			*byte = ((lo_nibble << 4) | (hi_nibble >> 4));
		}

		auto byte2 = ((uint8_t*)&val)[2];
		auto byte6 = ((uint8_t*)&val)[6];

		((uint8_t*)&val)[2] = byte6;
		((uint8_t*)&val)[6] = byte2;

		return val;
	}
}
