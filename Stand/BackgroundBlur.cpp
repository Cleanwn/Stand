#include "BackgroundBlur.hpp"

#include "Renderer.hpp"

namespace Stand
{
	void BackgroundBlur::drawH(float x, float y, float width, float height, uint8_t passes)
	{
		auto posC = g_renderer.posH2C(x, y);
		auto sizeC = g_renderer.sizeH2C(width, height);
		return drawC(posC.x, posC.y, sizeC.x, sizeC.y, passes);
	}

	void BackgroundBlur::drawC(float x, float y, float width, float height, uint8_t passes)
	{
		if (x >= 0
			&& y >= 0
			&& x < g_renderer.client_size.x
			&& y < g_renderer.client_size.y
			)
		{
			RECT r = RECT{ LONG(x), LONG(y), LONG(x + width), LONG(y + height) };

			if (r.left >= 0
				&& r.top >= 0
				&& r.right <= g_renderer.client_size.x
				&& r.bottom <= g_renderer.client_size.y
				)
			{
				g_renderer.leaveDrawContext();

				tex.fromBackbuffer(r);
				if (tex.hasUnderlyingTexture())
				{
					tex.blur(blurTmpTex, passes);

					g_renderer.enterDrawContext();
					g_renderer.m_pSpriteBatch->Draw(tex, r);
					g_renderer.leaveDrawContext();
				}
				g_renderer.enterDrawContext();
			}
		}
	}

	void BackgroundBlur::reset()
	{
		tex.reset();
		blurTmpTex.reset();
	}
}
