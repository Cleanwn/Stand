#pragma once

#include <soup/Rgb.hpp>
#include <soup/RenderTarget.hpp>

#include "huddecl.hpp"
#include "Renderer.hpp"

namespace Stand
{
	[[nodiscard]] inline DirectX::SimpleMath::Color rgb_soup2dx(soup::Rgb colour)
	{
		return DirectX::SimpleMath::Color(colour.r / 255.0f, colour.g / 255.0f, colour.b / 255.0f, 1.0f);
	}

	[[nodiscard]] inline soup::Rgb rgb_dx2soup(const DirectX::SimpleMath::Color& colour)
	{
		return soup::Rgb(colour.x * 255.0f, colour.y * 255.0f, colour.z * 255.0f);
	}

	struct RenderTargetC : public soup::RenderTarget
	{
		using soup::RenderTarget::RenderTarget;

		void drawRect(int x, int y, unsigned int width, unsigned int height, soup::Rgb colour) override
		{
			g_renderer.drawRectC((float)x, (float)y, (float)width, (float)height, rgb_soup2dx(colour));
		}
	};

	struct RenderTargetH : public soup::RenderTarget
	{
		RenderTargetH()
			: soup::RenderTarget(HUD_WIDTH, HUD_HEIGHT)
		{
		}

		void drawRect(int x, int y, unsigned int width, unsigned int height, soup::Rgb colour) override
		{
			g_renderer.drawRectH((float)x, (float)y, (float)width, (float)height, rgb_soup2dx(colour));
		}
	};

	struct RenderTargetWithOffsetH : public soup::RenderTarget
	{
		int x;
		int y;

		RenderTargetWithOffsetH(int x, int y)
			: soup::RenderTarget(HUD_WIDTH - x, HUD_HEIGHT - y), x(x), y(y)
		{
		}

		void drawRect(int x, int y, unsigned int width, unsigned int height, soup::Rgb colour) override
		{
			g_renderer.drawRectH((float)(x + this->x), (float)(y + this->y), (float)width, (float)height, rgb_soup2dx(colour));
		}
	};
}
