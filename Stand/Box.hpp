#pragma once

#include <soup/gmBoxCorners.hpp>

#include "fwddecl.hpp"
#include "Renderer.hpp"

namespace Stand
{
	struct Box : public soup::gmBoxCorners
	{
		using soup::gmBoxCorners::gmBoxCorners;

		void drawLines(int r, int g, int b, int a) const;
		void drawLines(const AbstractRenderer& renderer, DirectX::SimpleMath::Color& colour = g_renderer.arColour) const;
	};
}
