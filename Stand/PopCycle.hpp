#pragma once

#include "struct_base.hpp"

#include "atArray.hpp"

struct CPopGroupPercentage
{
	hash_t name;
	uint32_t percentage;
};
static_assert(sizeof(CPopGroupPercentage) == 0x08);

class CPopAllocation
{
	PAD(0, 0x20) rage::atArray<CPopGroupPercentage> m_vehGroupPercentages;
};
