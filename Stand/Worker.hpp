#pragma once

#include <functional>
#include <queue>

#include "Spinlock.hpp"

namespace Stand
{
	class Worker
	{
	private:
		static inline Spinlock jobs_mtx{};
		static inline std::queue<std::function<void()>> next_jobs{};

	public:
		static inline Spinlock root_update_mtx{};

		static void thread_func();

		static void queueJob(std::function<void()>&& func);
	};
}
