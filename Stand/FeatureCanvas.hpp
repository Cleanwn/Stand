#pragma once

#include <soup/fwd.hpp>

namespace Stand
{
	struct FeatureCanvas
	{
		using draw_t = void(*)(void* userdata, soup::RenderTarget&);

		virtual ~FeatureCanvas()
		{
			shutdown();
		}

		virtual void init(int width, int height, draw_t draw_func, void* userdata) = 0;
		virtual void shutdown() {}
	};
}
