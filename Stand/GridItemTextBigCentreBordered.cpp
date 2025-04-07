#include "GridItemTextBigCentreBordered.hpp"

#include "CommandboxGrid.hpp"
#include "Renderer.hpp"

namespace Stand
{
	GridItemTextBigCentreBordered::GridItemTextBigCentreBordered(std::wstring&& text, int16_t width, int16_t height, uint8_t priority, Alignment alignment_relative_to_last, GridItem* force_alignment_to)
		: GridItemTextBigCentre(std::move(text), width, height, priority, alignment_relative_to_last, force_alignment_to)
	{
	}

	void GridItemTextBigCentreBordered::draw()
	{
		GridItem::draw();
		g_renderer.drawInnerBorderH(x, y, width, height, CommandboxGrid::border_width, { 1.0f, 1.0f, 1.0f, 1.0f });
		drawText();
	}
}
