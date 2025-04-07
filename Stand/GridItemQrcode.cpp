#include "GridItemQrcode.hpp"

#include <soup/QrCode.hpp>

#include "ColourUtil.hpp"
#include "Renderer.hpp"
#include "RenderTarget.hpp"

namespace Stand
{
	struct RenderTargetQrcode : public RenderTargetH
	{
		using RenderTargetH::RenderTargetH;

		void drawRect(int x, int y, unsigned int width, unsigned int height, soup::Rgb colour) final
		{
			if (colour != soup::Rgb::BLACK)
			{
				RenderTargetH::drawRect(x, y, width, height, rgb_dx2soup(g_renderer.focusRectColour));
			}
		}
	};

	GridItemQrcode::GridItemQrcode(int16_t size, uint8_t priority, const std::string& text)
		: GridItem(GRIDITEM_INDIFFERENT, size, size, priority)
	{
		auto qr = soup::QrCode::encodeText(text);
		c = qr.toCanvas(2, soup::Rgb::WHITE, soup::Rgb::BLACK);
		c.resizeNearestNeighbour(width, height);
	}

	void GridItemQrcode::draw()
	{
		DirectX::SimpleMath::Color bg_colour{ 0.0f, 0.0f, 0.0f, 1.0f };
		if (!ColourUtil::isContrastSufficient(bg_colour, g_renderer.focusRectColour))
		{
			bg_colour.Negate();
		}
		drawBackground(bg_colour);

		RenderTargetQrcode rt;
		rt.drawCanvas(x, y, c);
	}
}
