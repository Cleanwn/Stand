#include "GridItemHeaderAnimation.hpp"

#include "Exceptional.hpp"
#include "get_current_time_millis.hpp"
#include "Renderer.hpp"

namespace Stand
{
	GridItemHeaderAnimation::GridItemHeaderAnimation(int16_t width, uint8_t priority, GridItem* force_alignment_to)
		: GridItemHeader(width, int16_t(
			g_renderer.header->getRenderHeight(width)
		), priority, force_alignment_to), header(
			g_renderer.header
		)
	{
	}

	void GridItemHeaderAnimation::draw()
	{
		if (g_renderer.header_bgblur)
		{
			drawBackgroundBlur();
		}
		if (g_renderer.header_i >= header->textures.size())
		{
			g_renderer.header_i = 0;
		}
		g_renderer.leaveDrawContext();
		g_renderer.enterSpriteDrawContextNonPremultiplied();
		__try
		{
			g_renderer.drawSpriteH(header->textures.at(g_renderer.header_i).Get(), x, y, width, height, DirectX::SimpleMath::Color(DirectX::Colors::White));
		}
		__EXCEPTIONAL()
		{
		}
		g_renderer.leaveSpriteDrawContextNonPremultiplied();
		g_renderer.enterDrawContext();
		const time_t this_frame = get_current_time_millis();
		if (g_renderer.header_last_frame != 0)
		{
			g_renderer.header_ms_passed += this_frame - g_renderer.header_last_frame;
		}
		g_renderer.header_last_frame = this_frame;
		while (g_renderer.header_ms_passed >= g_renderer.header_speed)
		{
			g_renderer.header_ms_passed -= g_renderer.header_speed;
			++g_renderer.header_i;
		}
	}
}
