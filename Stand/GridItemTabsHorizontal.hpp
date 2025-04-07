#pragma once

#include "GridItem.hpp"

namespace Stand
{
	class GridItemTabsHorizontal : public GridItem
	{
	public:
		explicit GridItemTabsHorizontal(uint8_t priority, Alignment alignment_relative_to_last, GridItem* force_alignment_to = nullptr);
		
		void draw() final;
	};
}
