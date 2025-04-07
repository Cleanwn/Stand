#include "ColoadMgr.hpp"

#ifdef STAND_DEV
#include "FileLogger.hpp"
#endif
#include "pointers.hpp"

namespace Stand
{
	void ColoadMgr::addCheck(const Codename& cn, void* addr, const void* data, size_t size)
	{
		std::vector<uint8_t> vec;
		vec.assign((const uint8_t*)data, (const uint8_t*)data + size);
		existing_hook_checks.emplace_front(ExistingHookCheck{ cn, addr, std::move(vec) });
	}

	void ColoadMgr::check()
	{
		if (coloading_with_any_menu)
		{
			return;
		}

		for (auto i = existing_hook_checks.head.load(); i != nullptr; i = i->next)
		{
			if (i->data.addr != nullptr && memcmp(i->data.addr, i->data.vec.data(), i->data.vec.size()) != 0)
			{
				coloading_with_any_menu = true;
#ifdef STAND_DEV
				g_logger.log(fmt::format(fmt::runtime(soup::ObfusString("Co-load detected ({})").str()), i->data.name.toString()));
#endif
				return;
			}
		}

		if (pointers::request_control_spectating_check)
		{
			if (pointers::request_control_spectating_check[-2] != 0x8B
				|| pointers::request_control_spectating_check[-1] != 0xD9
				|| pointers::request_control_spectating_check[0] != 0xE8
				|| pointers::request_control_spectating_check[7 - 2] != 0x84
				|| pointers::request_control_spectating_check[8 - 2] != 0xC0
				|| pointers::request_control_spectating_check[9 - 2] != 0x75
				|| pointers::request_control_spectating_check[10 - 2] != 0x6A
				|| pointers::request_control_spectating_check[11 - 2] != 0x8B
				|| pointers::request_control_spectating_check[12 - 2] != 0xCB
				|| pointers::request_control_spectating_check[13 - 2] != 0xE8
				)
			{
				coloading_with_any_menu = true;
				if (pointers::request_control_spectating_check[-2] == 0x8B
					&& pointers::request_control_spectating_check[-1] == 0xD9
					&& pointers::request_control_spectating_check[0] == 0xE8
					&& pointers::request_control_spectating_check[7 - 2] == 0x90
					&& pointers::request_control_spectating_check[8 - 2] == 0x90
					&& pointers::request_control_spectating_check[9 - 2] == 0x90
					&& pointers::request_control_spectating_check[10 - 2] == 0x90
					&& pointers::request_control_spectating_check[11 - 2] == 0x8B
					&& pointers::request_control_spectating_check[12 - 2] == 0xCB
					&& pointers::request_control_spectating_check[13 - 2] == 0xE8
					)
				{
					coloading_with_cherax = true;
#ifdef STAND_DEV
					g_logger.log(fmt::format(fmt::runtime(soup::ObfusString("Co-load detected ({})").str()), Codename("HC").toString()));
#endif
				}
				else
				{
#ifdef STAND_DEV
					g_logger.log(fmt::format(fmt::runtime(soup::ObfusString("Co-load detected ({})").str()), Codename("HD").toString()));
#endif
				}
			}
		}
	}
}
