#pragma once

#include "ScriptGlobal.hpp"

namespace Stand
{
	struct StatCache
	{
		static inline int rank = 0;

		static void onTick()
		{
			if (is_session_started()) // MP Stats
			{
				const auto char_slot = ScriptGlobal(GLOBAL_CHAR_SLOT).get<int>();
				STATS::STAT_GET_INT(CHAR_STAT(char_slot, "CHAR_RANK_FM"), &rank, -1);
			}
			// Note that SP Stats may need to be explicitly loaded before getting accessed.
		}
	};
}
