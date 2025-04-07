#pragma once

#include "Component.hpp"

namespace Stand
{
	struct ComponentGpu : public Component
	{
		void addPatterns(PatternBatch& batch) final;
		[[nodiscard]] std::vector<DetourHook*> getHooks() final;

		time_t overhead_ms = -1;
	};
	inline ComponentGpu g_comp_gpu;
}
