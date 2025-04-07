#include "GridItemColourBoxBordered.hpp"

#include "CommandboxGrid.hpp"
#include "Renderer.hpp"

namespace Stand
{
	GridItemColourBoxBordered::GridItemColourBoxBordered(uint8_t priority, Alignment alignment_relative_to_last, DirectX::SimpleMath::Color&& colour, DirectX::SimpleMath::Color&& border_colour)
		: GridItemColourBox(priority, alignment_relative_to_last, std::move(colour)), border_colour(std::move(border_colour))
	{
	}

	void GridItemColourBoxBordered::draw()
	{
		drawBackgroundRect(border_colour);
		g_renderer.drawRectH(float(x + CommandboxGrid::border_width), float(y + CommandboxGrid::border_width), float(width - (CommandboxGrid::border_width * 2)), float(height - (CommandboxGrid::border_width * 2)), colour);
	}
}
