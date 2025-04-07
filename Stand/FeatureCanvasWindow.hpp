#pragma once

#include "FeatureCanvas.hpp"

#include <optional>

#include <soup/Window.hpp>

namespace Stand
{
	struct FeatureCanvasWindow : public FeatureCanvas
	{
		std::optional<soup::Window> w;

		void init(int width, int height, draw_t draw_func, void* userdata) final;
		void shutdown() final;
	};
}
