#pragma once

#include <string>

#include "typedecl.hpp"

namespace Stand
{
	extern std::wstring format_time_since_1970_for_user_locale(time_t time, bool only_time = false);
}
