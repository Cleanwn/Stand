#pragma once

#include "AbstractPlayer.hpp"

namespace Stand
{
	struct GlobalsWatcher
	{
		inline static bool notify_thunder_request = true;
		inline static playerbitset_t did_notify_thunder = 0;

		static void onTickSessionActive();
		static void processIndirectPickup(const AbstractPlayer& source, hash_t type, int value, hash_t model);
	};
}
