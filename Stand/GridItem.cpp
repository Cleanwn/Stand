#include "GridItem.hpp"

#include "Renderer.hpp"

namespace Stand
{
	GridItem::GridItem(GridItemType type, int16_t width, int16_t height, uint8_t priority, Alignment alignment_relative_to_last, GridItem* force_alignment_to)
		: type(type), width(width), height(height), priority(priority), alignment_relative_to_last(alignment_relative_to_last), force_alignment_to(force_alignment_to)
	{
	}

	bool GridItem::occupies(int16_t x, int16_t y) const
	{
		return this->x <= x && x <= (this->x + this->width)
			&& this->y <= y && y <= (this->y + this->height);
	}

	void GridItem::onPositioned()
	{
	}

	void GridItem::draw()
	{
		drawBackground(g_renderer.getBgRectColour());
	}

	void GridItem::drawBackground(const DirectX::SimpleMath::Color& colour)
	{
		drawBackgroundEffects(colour);
		drawBackgroundRect(colour);
	}

	void GridItem::drawBackgroundRect(const DirectX::SimpleMath::Color& colour) const
	{
		g_renderer.drawRectH(x, y, width, height, colour);
	}

	void GridItem::drawBackgroundEffects()
	{
		drawBackgroundEffects(g_renderer.getBgRectColour());
	}

	void GridItem::drawBackgroundEffects(const DirectX::SimpleMath::Color& bgColour)
	{
		if (bgColour.A() != 1.0f)
		{
			drawBackgroundBlur();
		}
	}

	void GridItem::drawBackgroundBlur()
	{
		if (g_renderer.background_blur != 0)
		{
			bgblur.drawH(x, y, width, height, g_renderer.background_blur);
		}
	}
}
