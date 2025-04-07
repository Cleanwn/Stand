#include "GridItemCommandboxInput.hpp"

#include "Renderer.hpp"

namespace Stand
{
	GridItemCommandboxInput::GridItemCommandboxInput(std::wstring&& text, int16_t width, uint8_t priority)
		: GridItem(GRIDITEM_INDIFFERENT, width, 0, priority), text(g_renderer.wrapTextH(std::move(text), g_renderer.commandbox_input_text.scale, float(width - 10)))
	{
		height = int16_t(g_renderer.getTextHeight(this->text, g_renderer.commandbox_input_text.scale) + 5);
	}

	void GridItemCommandboxInput::draw()
	{
		GridItem::draw();
		g_renderer.drawTextH(float(x + 5), float(y), text.c_str(), g_renderer.getBgTextColour(), g_renderer.commandbox_input_text);
	}
}
