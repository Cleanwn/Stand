#pragma once

#include <cstdint>

namespace Stand
{
	struct TextColour
	{
		wchar_t code;
		uint8_t r;
		uint8_t g;
		uint8_t b;

		constexpr TextColour(wchar_t code, uint32_t hex)
			: code(code), r(hex >> 16), g(hex >> 8), b(hex)
		{
		}

		static TextColour all[11];
	};
}
