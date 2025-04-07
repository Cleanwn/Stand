#pragma once

#include "gta_extensible.hpp"

#pragma pack(push, 1)
class CPedHeadBlendData : public rage::fwExtension
{
	INIT_PAD(rage::fwExtension, 0x028) float shapeMix;
	/* 0x02C */ float skinMix;
	/* 0x030 */ float thirdMix;
	PAD(0x030 + 4, 0x0A0) float m_microMorphBlends[20];
	PAD(0x0A0 + (sizeof(float) * 20), 0x117) uint8_t shapeFirst;
	/* 0x118 */ uint8_t shapeSecond;
	/* 0x119 */ uint8_t shapeThird;
	/* 0x11A */ uint8_t skinFirst;
	/* 0x11B */ uint8_t skinSecond;
	/* 0x11C */ uint8_t skinThird;
	PAD(0x11C + 1, 0x138) uint8_t hair_colour_id;
	/* 0x139 */ uint8_t hair_highlight_colour_id;
	PAD(0x139 + 1, 0x13D) uint8_t possibly_isParent;
};
static_assert(sizeof(CPedHeadBlendData) == 0x13D + 1);
#pragma pack(pop)
