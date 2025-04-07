#pragma once

#include "GridItemTextBigCentre.hpp"

namespace Stand
{
	class GridItemTextBigCentreBordered : public GridItemTextBigCentre
	{
	public:
		explicit GridItemTextBigCentreBordered(std::wstring&& text, int16_t width, int16_t height, uint8_t priority, Alignment alignment_relative_to_last, GridItem* force_alignment_to);

		void draw() final;
	};
}
