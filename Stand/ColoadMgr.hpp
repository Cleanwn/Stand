#pragma once

#include <vector>

#include <soup/AtomicStack.hpp>

#include "Codename.hpp"

namespace Stand
{
	struct ExistingHookCheck
	{
		Codename name;
		void* addr = nullptr;
		std::vector<uint8_t> vec;
	};

	struct ColoadMgr
	{
		inline static bool coloading_with_any_menu = false;
		inline static bool coloading_with_2take1menu = false;
		inline static bool coloading_with_cherax = false;
		inline static soup::AtomicStack<ExistingHookCheck> existing_hook_checks{};

		static void addCheck(const Codename& cn, void* addr, const void* data, size_t size);

		static void check();
	};
}
