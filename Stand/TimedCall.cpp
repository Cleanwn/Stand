#include "TimedCall.hpp"

#include <cstdint>

#include <fmt/core.h>
#include <Windows.h>

#include "drawDebugText.hpp"
#include "FileLogger.hpp"

namespace Stand
{
	std::string TimedCall::run(const std::function<void()>& func)
	{
		uint64_t ticks;
		QueryPerformanceCounter((LARGE_INTEGER*)&ticks);

		func();

		uint64_t ticks2;
		QueryPerformanceCounter((LARGE_INTEGER*)&ticks2);

		ticks2 -= ticks;

		auto ms = (ticks2 / 10000);
		auto fraction = (ticks2 % 10000);

		return fmt::format("{}.{:0>5} ms", ms, fraction);
	}

	std::string TimedCall::run(const std::string& name, const std::function<void()>& func)
	{
		return fmt::format("{}: {}", name, run(func));
	}

	void TimedCall::tick(const std::string& name, const std::function<void()>& func)
	{
		drawDebugText(run(name, func));
	}

	void TimedCall::once(const std::string& name, const std::function<void()>& func)
	{
		g_logger.log(run(name, func));
	}

	void TimedCall::log(const std::string& format_string, const std::function<void()>& func)
	{
		g_logger.log(fmt::format(fmt::runtime(format_string), run(func)));
	}
}
