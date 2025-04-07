#pragma once

#include <functional>

#include "ThreadContext.hpp"

namespace Stand
{
	struct ExecCtx
	{
		ThreadContext tc = TC_OTHER;
		bool script_tick_event_mtx_is_locked = false;

		[[nodiscard]] static ExecCtx& get() noexcept;

		[[nodiscard]] bool isScript() const noexcept;
		[[nodiscard]] bool hasGameTls() const noexcept;

		void ensureScript(std::function<void()>&& f);

		void yield();
	};
}
