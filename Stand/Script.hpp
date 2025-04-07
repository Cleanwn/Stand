#pragma once

#include <optional>
#include <functional>
#include <Windows.h>

#include "typedecl.hpp"

#ifdef STAND_DEBUG
#define REPORT_YIELD_IN_NOYIELD true
#else
#define REPORT_YIELD_IN_NOYIELD false
#endif

#ifdef STAND_DEBUG
#define REPORT_YIELD_WITH_LOCK true
#else
#define REPORT_YIELD_WITH_LOCK false
#endif

namespace Stand
{
	using script_func_t = void(*)();

	class Script
	{
	public:
		inline static void* ret_fiber;

		script_func_t func;
	private:
		void* fiber;

	private:
#if REPORT_YIELD_WITH_LOCK
		bool nested = false;
#endif
	
	public:
		explicit Script(script_func_t func);
		~Script();

		[[nodiscard]] static Script* current();

		[[nodiscard]] HANDLE getModule() const;

		[[nodiscard]] bool isCurrent() const noexcept;

		void stop();

		bool tick();
		void nestedTick();
		void yield();
		void yield(time_t minSleepMs);

	private:
		void fiberFunc();
	};
}
