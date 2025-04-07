#pragma once

#include "rtti_shim.hpp"

namespace rage
{
	class datBase
	{
	public:
		RAGE_RTTI_SHIM
		virtual ~datBase() = default; // 0x30
	};
	static_assert(sizeof(datBase) == 0x08);
}
