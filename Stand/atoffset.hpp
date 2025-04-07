#pragma once

#include <cstdint>

namespace Stand
{
	template <typename T>
	[[nodiscard]] constexpr T* atoffset(void* obj, uintptr_t offset)
	{
		return reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(obj) + offset);
	}
}
