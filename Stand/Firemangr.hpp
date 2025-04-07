#pragma once

#include <deque>

#include "natives_decl.hpp"

namespace Stand
{
	struct Firemangr
	{
		inline static std::deque<FireId> script_fires{};
	};
}
