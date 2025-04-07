#pragma once

namespace Stand
{
	struct RootMgr
	{
		static void updateRootState();
		static void updateRootStateImpl();
		static void updateRootStateCore();
	};
}
