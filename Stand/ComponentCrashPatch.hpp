#pragma once

#include "Component.hpp"

#include "AbstractPlayer.hpp"

namespace Stand
{
	struct ComponentCrashPatch : public Component
	{
		bool silently_block_invalid_mechanical_part = false;

		void addPatterns(PatternBatch& batch) final;
		[[nodiscard]] std::vector<DetourHook*> getHooks() final;
	};
	inline ComponentCrashPatch g_comp_crashpatch;

	inline AbstractPlayer sync_src;
}
