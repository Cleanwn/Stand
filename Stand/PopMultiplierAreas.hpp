#pragma once

#include "vector.hpp"

#define MAX_POP_MULTIPLIER_AREAS (15)

struct __declspec(align(8)) PopMultiplierArea
{
	/* 0x00 */ rage::Vector3 m_minWS;
	/* 0x10 */ rage::Vector3 m_maxWS;
	/* 0x20 */ float m_pedDensityMultiplier;
	/* 0x24 */ float m_trafficDensityMultiplier;
	/* 0x28 */ bool m_init;
	/* 0x29 */ bool m_isSphere;
	/* 0x2A */ bool m_bCameraGlobalMultiplier;

	// For sphere areas
	[[nodiscard]] float GetRadius() const
	{
		return m_maxWS.w;
	}
};
static_assert(sizeof(PopMultiplierArea) == 0x30);
