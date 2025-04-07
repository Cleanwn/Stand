#pragma once

#include <Windows.h>

namespace Stand
{
	struct WorldState
	{
		[[nodiscard]] static BOOL getOnline();
		static void setOnline(BOOL toggle);
	};
}
