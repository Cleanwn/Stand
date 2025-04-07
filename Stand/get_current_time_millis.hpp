#pragma once

#include "typedecl.hpp"

namespace Stand
{
	[[nodiscard]] extern time_t get_current_time_millis() noexcept;
}

#define GET_MILLIS_SINCE(since) (::Stand::get_current_time_millis() - (since))
#define IS_DEADLINE_REACHED(deadline) (::Stand::get_current_time_millis() >= deadline)
