#pragma once

#include "typedecl.hpp"

#include "Exceptional.hpp"
#include "get_current_time_millis.hpp"

#define ANTIDBG_ENABLED !STAND_DEBUG || false

namespace Stand
{
	class AntiDbg
	{
	private:
		inline static time_t last_debugger_check = 0;

	public:
		[[nodiscard]] static __forceinline bool wasDebuggerCheckedRecently()
		{
			return GET_MILLIS_SINCE(AntiDbg::last_debugger_check) <= 20000;
		}

		[[nodiscard]] static constexpr bool isEnabled() noexcept
		{
#if ANTIDBG_ENABLED
			return true;
#else
			return false;
#endif
		}

		static __forceinline void crash()
		{
			Exceptional::disableExceptionHandling();
			ZeroMemory(0, -1);
		}

		static void apologiseInAdvance(int code);
		static void enqueueSuspension(uintptr_t data);
		static void bToTheSToTheOToTheD();
		[[nodiscard]] static __declspec(noinline) uint8_t getRunningDebuggerId(bool passive);
	};
}
