#pragma once

#include "fwddecl.hpp"

#include <vector>

namespace Stand
{
	struct Component
	{
		virtual void addPatterns(PatternBatch& batch)
		{
		}

		[[nodiscard]] virtual std::vector<DetourHook*> getHooks()
		{
			return {};
		}
	};
}
