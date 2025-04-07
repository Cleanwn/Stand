#pragma once

#include "TimedTextCollection.hpp"

namespace Stand
{
	struct TimedTextPresenter
	{
		TimedTextCollection presentation{};
		time_t started_at{};

		void startPresenting(TimedTextCollection&& c) noexcept;
		[[nodiscard]] TimedText getCurrentText() noexcept;
	};
}
