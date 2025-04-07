#pragma once

#include "tlsContext.hpp"

namespace rage
{
	struct sysThreadType
	{
		enum
		{
			THREAD_TYPE_UPDATE = 1 << 0,
			THREAD_TYPE_RENDER = 1 << 1,
			THREAD_TYPE_PROCESS_MAIN = 1 << 2,
			THREAD_TYPE_DEPENDENCY = 1 << 3,

			USER_THREAD_TYPE_SHIFT = 4,
		};

		[[nodiscard]] static int GetCurrentThreadType()
		{
			return tlsContext::get()->thisThreadType;
		}

		[[nodiscard]] static bool IsUpdateThread()
		{
			return (GetCurrentThreadType() & THREAD_TYPE_UPDATE) != 0;
		}

		[[nodiscard]] static bool IsRenderThread()
		{
			return (GetCurrentThreadType() & THREAD_TYPE_RENDER) != 0;
		}
	};
}
