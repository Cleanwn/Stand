#pragma once

#include <cstdint>

namespace rage
{
	struct atString
	{
		char* data;
		uint16_t size;
		uint16_t capacity;
	};
	static_assert(sizeof(atString) == 0x10);
}
