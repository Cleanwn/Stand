#pragma once

#include "netLoggingInterface.hpp"

#include <cstdarg>

#include <fmt/core.h>

#include "Exceptional.hpp"
#include "FileLogger.hpp"

namespace Stand
{
	class NetLoggingInterfaceShim : public rage::netLoggingInterface
	{
	private:
		const char* component;

	public:
		explicit NetLoggingInterfaceShim(const char* component)
			: component(component)
		{
		}

		void _0x30(const char* label, const char* data_fmt, ...) final
		{
			__try
			{
				va_list argp;
				va_start(argp, data_fmt);
				char buffer[256];
				vsprintf_s(buffer, data_fmt, argp);
				va_end(argp);
				g_logger.log(fmt::format("[{}] {}: {}", component, label, (const char*)&buffer[0]));
			}
			__EXCEPTIONAL()
			{
			}
		}
	};
}
