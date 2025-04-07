#pragma once

#include "Component.hpp"

namespace Stand
{
	struct ComponentDrawPatch : public Component
	{
		void addPatterns(PatternBatch& batch) final;
		[[nodiscard]] std::vector<DetourHook*> getHooks() final;

		bool enabled = true;

		bool block_all = false;
	};
	inline ComponentDrawPatch g_comp_drawpatch{};
}
