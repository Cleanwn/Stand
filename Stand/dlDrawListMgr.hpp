#pragma once

#include "struct_base.hpp"

namespace rage
{
	struct dlDrawListMgr
	{
		struct Slot // made up
		{
			PAD(0, 0xA0)
		};

		PAD(0, 0x730) Slot slots[512];
		/* 0x14730 */ uint32_t free_slot;

		[[nodiscard]] bool isExhausted() const noexcept
		{
			return free_slot == 512;
		}
	};
	static_assert(sizeof(dlDrawListMgr) == 0x14730 + 4);
}

struct CDrawListMgr : public rage::dlDrawListMgr
{
};
