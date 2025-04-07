#pragma once

#include "crSkeleton.hpp"

namespace rage
{
	struct fragHierarchyInst
	{
		PAD(0, 0x30) crSkeleton* skeleton;
	};

	struct fragCacheEntry
	{
		PAD(0, 0x148) fragHierarchyInst m_HierInst;
	};
}
