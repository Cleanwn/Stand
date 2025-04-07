#pragma once

#include <Windows.h>

namespace rage
{
	struct sysCriticalSection
	{
		_RTL_CRITICAL_SECTION data;

		void Enter()
		{
			if (data.DebugInfo)
			{
				EnterCriticalSection(&data);
			}
		}

		void Leave()
		{
			if (data.DebugInfo)
			{
				LeaveCriticalSection(&data);
			}
		}
	};
	static_assert(sizeof(sysCriticalSection) == sizeof(_RTL_CRITICAL_SECTION));

	struct sysCriticalSectionToken
	{
		uint64_t m_Impl[5];
	};
	static_assert(sizeof(sysCriticalSectionToken) == 0x28);
}
