#pragma once

#include "GridItem.hpp"

#include "Texture.hpp"

namespace Stand
{
	class GridItemHeader : public GridItem
	{
	public:
		explicit GridItemHeader(int16_t width, int16_t height, uint8_t priority, GridItem* force_alignment_to);

		void draw() override = 0;
	};
}
