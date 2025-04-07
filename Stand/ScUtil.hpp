#pragma once

namespace Stand
{
	struct ScUtil
	{
		[[nodiscard]] static bool isBlocked(int64_t rid);
		static void block(int64_t rid);
		static void unblock(int64_t rid);
	};
}
