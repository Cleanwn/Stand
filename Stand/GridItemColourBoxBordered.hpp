#pragma once

#include "GridItemColourBox.hpp"

namespace Stand
{
	class GridItemColourBoxBordered : public GridItemColourBox
	{
	private:
		const DirectX::SimpleMath::Color border_colour;

	public:
		explicit GridItemColourBoxBordered(uint8_t priority, Alignment alignment_relative_to_last, DirectX::SimpleMath::Color&& colour, DirectX::SimpleMath::Color&& border_colour);

		void draw() final;
	};
}
