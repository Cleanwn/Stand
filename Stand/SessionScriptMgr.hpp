#pragma once

#include "fwddecl.hpp"
#include "hashtype.hpp"

namespace Stand
{
	struct SessionScriptMgr
	{
		inline static bool busy = false;
		[[nodiscard]] static bool startPrecheck(Click& click);
		static void start(const hash_t hash); // needs a yieldable script thread

		[[nodiscard]] static int hash_to_id(const hash_t hash);
		[[nodiscard]] static hash_t id_to_hash(const int id);
	};
}
