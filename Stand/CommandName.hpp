#pragma once

#include <string>

#define COMPACT_COMMAND_NAMES true

namespace Stand
{
#if COMPACT_COMMAND_NAMES
	using CommandName = std::string;
#else
	using CommandName = std::wstring;
#endif
}
