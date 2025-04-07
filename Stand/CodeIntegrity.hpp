#pragma once

namespace Stand
{
	struct CodeIntegrity
	{
		static void add(void* func);
		static bool verify();

		[[nodiscard]] static uint32_t hashFunction(const void* func);
	};
}
