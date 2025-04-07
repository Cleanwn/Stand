#pragma once

#include <functional>
#include <string>

namespace Stand
{
	struct TimedCall
	{
		[[nodiscard]] static std::string run(const std::function<void()>& func);
		[[nodiscard]] static std::string run(const std::string& name, const std::function<void()>& func);

		static void tick(const std::string& name, const std::function<void()>& func);
		static void once(const std::string& name, const std::function<void()>& func);

		static void log(const std::string& format_string, const std::function<void()>& func);
	};
}
