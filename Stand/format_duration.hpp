#pragma once

#include <string>

#include "fwddecl.hpp"

namespace Stand
{
	[[nodiscard]] extern std::string format_duration(time_t ms);
}
