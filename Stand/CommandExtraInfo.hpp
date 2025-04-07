#pragma once

#include <cstdint>
#include <string>

namespace Stand
{
	struct CommandExtraInfo
	{
		std::wstring completed_hint{};
		uint8_t char_limit = 0;
		bool colour_selector = false;
		bool collapse = false;
	};
}
