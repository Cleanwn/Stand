#include "GridItemPrimaryText.hpp"

#include "Renderer.hpp"

namespace Stand
{
	GridItemPrimaryText::GridItemPrimaryText(int16_t width, int16_t height, std::wstring&& text)
		: GridItem(GRIDITEM_PRIMARYTEXT, width, height, 1), text(std::move(text))
	{
	}

	void GridItemPrimaryText::draw()
	{
		draw(g_renderer.small_text);
	}

	void GridItemPrimaryText::draw(const TextSettings& settings)
	{
		drawBackground(g_renderer.getFocusRectColour());
		g_renderer.drawTextH(float(x + 5), float(y), text, g_renderer.getFocusTextColour(), settings);
	}
}
