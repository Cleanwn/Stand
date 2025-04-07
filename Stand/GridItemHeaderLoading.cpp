#include "GridItemHeaderLoading.hpp"

#include <fmt/xchar.h>

#include "Renderer.hpp"

namespace Stand
{
	GridItemHeaderLoading::GridItemHeaderLoading(int16_t width, uint8_t priority, GridItem* force_alignment_to)
		: GridItemHeader(width, int16_t(g_renderer.textureHeaderLoading.getRenderHeight(width)), priority, force_alignment_to)
	{
	}

	void GridItemHeaderLoading::draw()
	{
		GridItem::draw();
		if (g_renderer.header_goal != 0)
		{
			auto progress = fmt::format(L"{}/{}", g_renderer.header_progress, g_renderer.header_goal);
			g_renderer.drawTextSmallH(float(x + width - 10), float(y + 5), progress.c_str(), g_renderer.getBgTextColour(), ALIGN_TOP_RIGHT);
		}
		g_renderer.drawSpriteH(g_renderer.textureHeaderLoading, x, y, width, height, DirectX::SimpleMath::Color(DirectX::Colors::White));
	}
}
