#include "GridItemText.hpp"

#include "Renderer.hpp"

namespace Stand
{
	GridItemText::GridItemText(std::wstring&& text, int16_t width, int16_t extra_padding, uint8_t priority, Alignment alignment_relative_to_last, GridItem* force_alignment_to)
		: GridItem(GRIDITEM_INDIFFERENT, width + extra_padding, extra_padding, priority, alignment_relative_to_last, force_alignment_to),
		  text(g_renderer.wrapTextH(std::move(text), g_renderer.small_text.scale, float(width - (10 + extra_padding)))),
		  extra_padding(extra_padding)
	{
		height += int16_t(g_renderer.getTextHeight(this->text, g_renderer.small_text.scale) + 5 + extra_padding);
	}

	void GridItemText::draw()
	{
		GridItem::draw();
		drawText();
	}

	void GridItemText::drawText() const
	{
		g_renderer.drawTextSmallH(float(x + 5 + extra_padding), float(y + extra_padding), text.c_str(), g_renderer.getBgTextColour());
	}
}
