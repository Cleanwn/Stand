#pragma once

#include <string>

#include "typedecl.hpp"

namespace Stand
{
	struct TimedText
	{
		time_t offset_millis;
		std::string text;
	};
}
