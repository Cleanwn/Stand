#include "GridItemColourBox.hpp"

#include "CommandboxGrid.hpp"

namespace Stand
{
	GridItemColourBox::GridItemColourBox(uint8_t priority, Alignment alignment_relative_to_last, DirectX::SimpleMath::Color&& colour)
		: GridItem(GRIDITEM_INDIFFERENT, CommandboxGrid::colour_size, CommandboxGrid::colour_size, priority, alignment_relative_to_last), colour(std::move(colour))
	{
	}

	void GridItemColourBox::draw()
	{
		drawBackgroundRect(colour);
	}
}
