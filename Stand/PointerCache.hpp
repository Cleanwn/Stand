#pragma once

#include <string>
#include <unordered_map>

namespace Stand
{
	struct PointerCache
	{
		inline static constexpr uintptr_t xor_magic = 24074753;
		inline static std::wstring file{};
		inline static std::unordered_map<std::string, uintptr_t> cache{};

		[[nodiscard]] static bool isInited();
		static void init();
		static void save();
		static void deinit();
		static void saveAndDeinit();
	};
}
