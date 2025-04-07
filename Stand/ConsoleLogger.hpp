#pragma once

#include "Logger.hpp"

#include <fstream>

#include <soup/DetachedScheduler.hpp>

#include "Spinlock.hpp"

namespace Stand
{
	class ConsoleLogger : public Logger
	{
	public:
		std::ofstream out{};
		Spinlock sched_mtx{};
		soup::DetachedScheduler sched{};

		ConsoleLogger();

		[[nodiscard]] bool isInited() const;
		[[nodiscard]] static bool gameHasAnyConsole();
		void init();
		void deinit();

	protected:
		void write(std::string&& message) final;
	};

	inline ConsoleLogger g_console{};
}
