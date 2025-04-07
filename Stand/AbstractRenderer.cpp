#include "AbstractRenderer.hpp"

#include "DrawUtil3d.hpp"
#include "vector.hpp"

namespace Stand
{
	void AbstractRendererDx::drawLine(const rage::Vector2& pos1, const rage::Vector2& pos2, DirectX::SimpleMath::Color& colour) const
	{
		g_renderer.drawLineCP(pos1, pos2, colour);
	}

	void AbstractRendererDx::drawLine(const v3& pos1, const v3& pos2, DirectX::SimpleMath::Color& colour) const
	{
		g_renderer.drawLineCP(pos1.getScreenPos(), pos2.getScreenPos(), colour);
	}

	void AbstractRendererDx::drawText(const v3& pos, std::wstring&& text, float scale, DirectX::SimpleMath::Color& colour) const
	{
		const rage::Vector2 screen_pos = pos.getScreenPos();
		if (screen_pos.isInBounds())
		{
			auto posC = g_renderer.CP2C(screen_pos.x, screen_pos.y);
			g_renderer.drawTextC(posC.x, posC.y, std::move(text), ALIGN_CENTRE, scale * g_renderer.resolution_text_scale, colour, g_renderer.m_font_user, false);
		}
	}

	void AbstractRendererNative::drawLine(const rage::Vector2& pos1, const rage::Vector2& pos2, DirectX::SimpleMath::Color& colour) const
	{
		g_renderer.drawLineThisTickCP(pos1, pos2, colour);
	}

	void AbstractRendererNative::drawLine(const v3& pos1, const v3& pos2, DirectX::SimpleMath::Color& colour) const
	{
		DrawUtil3d::draw_line_native(pos1, pos2, colour);
	}

	void AbstractRendererNative::drawText(const v3& pos, std::wstring&& text, float scale, DirectX::SimpleMath::Color& colour) const
	{
		g_renderer.drawUnscaled3dTextThisTickH(pos, std::move(text), scale, colour);
	}
}
