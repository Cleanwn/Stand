#pragma once

#include "gta_extensible.hpp"

#include "Mat34V.hpp"

namespace rage
{
	class fwAltSkeletonExtension : public fwExtension
	{
	public:
		/* 0x08 */ fwEntity* m_entity;
		/* 0x10 */ Mat34V m_ofset;
	};
	static_assert(sizeof(fwAltSkeletonExtension) == 0x50);
}
