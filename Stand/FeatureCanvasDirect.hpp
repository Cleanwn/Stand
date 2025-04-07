#pragma once

#include "FeatureCanvas.hpp"

namespace Stand
{
	class FeatureCanvasDirect : public FeatureCanvas
	{
	private:
		bool enabled = false;

	public:
		void init(int width, int height, draw_t draw_func, void* userdata) final;
		void shutdown() final;
	};
}
