#pragma once

#include "fwddecl.hpp"

#include <vector>

namespace Stand
{
	struct Components
	{
		static void addPatterns(PatternBatch& batch);
		static void collectHooks(std::vector<DetourHook*>& hooks);
	};
}
