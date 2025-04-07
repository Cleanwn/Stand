#pragma once

#include <cstdint>

namespace rage
{
	enum class SyncType : uint32_t
	{
		CREATE  = 1 << 0,
		UPDATE  = 1 << 1,
		CONTROL = 1 << 2,
	};
}
