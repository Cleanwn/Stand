#pragma once

#include <functional>

namespace Stand
{
	struct FiberPool
	{
		static void queueJob(std::function<void()>&& func);
		static void onTick();
	};
}
