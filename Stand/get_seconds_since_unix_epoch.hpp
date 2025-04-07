#pragma once

#include "typedecl.hpp"

namespace Stand
{
	extern unixtime_t get_seconds_since_unix_epoch() noexcept;
}

#define GET_SECONDS_SINCE(since) (::Stand::get_seconds_since_unix_epoch() - (since))
