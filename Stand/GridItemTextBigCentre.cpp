#include "GridItemTextBigCentre.hpp"

#include "Renderer.hpp"

namespace Stand
{
	GridItemTextBigCentre::GridItemTextBigCentre(std::wstring&& text, int16_t width, int16_t height, uint8_t priority, Alignment alignment_relative_to_last, GridItem* force_alignment_to)
		: GridItem(GRIDITEM_INDIFFERENT, width, height, priority, alignment_relative_to_last, force_alignment_to), text(std::move(text))
	{
	}

	void GridItemTextBigCentre::draw()
	{
		GridItem::draw();
		drawText();
	}

	void GridItemTextBigCentre::drawText() const
	{
		g_renderer.drawTextH(float(x + (width / 2) + 1), float(y + (height / 2) - 2), text.c_str(), g_renderer.getBgTextColour(), g_renderer.command_text, ALIGN_CENTRE);
	}
}
