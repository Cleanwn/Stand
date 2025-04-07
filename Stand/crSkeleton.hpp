#pragma once

#include "struct_base.hpp"

#include "Mat34V.hpp"

namespace rage
{
#pragma pack(push, 1)
	class crSkeleton
	{
		PAD(0x00, 0x08) rage::Mat34V* parent;
		PAD(0x10, 0x18) rage::Mat34V* objects;
		/* 0x20 */ uint32_t num_bones;
	};
	static_assert(sizeof(crSkeleton) == 0x20 + 4);
#pragma pack(pop)
}
