#pragma once

#include <string>

namespace Stand
{
	// May use PDB info if it's loaded:
	// MyStackWalker::getInstance()->LoadModules();
	struct PointerNames
	{
		[[nodiscard]] static std::string format(void* addr);
		[[nodiscard]] static std::string getName(void* addr, bool use_debug_symbols = true);
		static void registerFunctionName(void* addr, std::string name);
	};
}
