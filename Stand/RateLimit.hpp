#pragma once

#include "get_current_time_millis.hpp"

namespace Stand
{
#pragma pack(push, 1)
	class RateLimit
	{
	public:
		time_t window_duration_ms;
		unsigned int max_window_requests;

		time_t window_end = 0;
		unsigned int window_reqs = 0;

		constexpr RateLimit(time_t window_duration_ms, unsigned int max_window_requests)
			: window_duration_ms(window_duration_ms), max_window_requests(max_window_requests)
		{
		}

		[[nodiscard]] bool isInWindow() const noexcept
		{
			return get_current_time_millis() < window_end;
		}

		[[nodiscard]] bool canRequest() const noexcept
		{
			return !isInWindow() || (window_reqs < max_window_requests);
		}

		void addRequest() noexcept
		{
			if (isInWindow())
			{
				++window_reqs;
			}
			else
			{
				window_end = get_current_time_millis() + window_duration_ms;
				window_reqs = 1;
			}
		}
	};
#pragma pack(pop)
}
