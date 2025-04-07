#pragma once

#include "struct_base.hpp"

#include <Windows.h>

#pragma pack(push, 1)
namespace rage
{
	class tlsContext
	{
	public:
		PAD(0, 0xB4) int thisThreadType; // CViewportManager pattern in main
		/* 0xB8 */ sysMemAllocator* sysMemAllocator_sm_Current;
		/* 0xC0 */ sysMemAllocator* sysMemAllocator_sm_Master;
		/* 0xC8 */ sysMemAllocator* sysMemAllocator_sm_Container;
		PAD(0xC8 + 8, 0x858) scrThread* m_script_thread; // 74 2C E8 ? ? ? ? FF 4B 10 + 3 rip = rage::scrThread::GetActiveThread
		/* 0x860 */ bool m_is_script_thread_active;

		[[nodiscard]] static tlsContext* get()
		{
			return *reinterpret_cast<tlsContext**>(__readgsqword(0x58));
		}
	};
	static_assert(sizeof(tlsContext) == 0x860 + sizeof(bool));
}
#pragma pack(pop)
