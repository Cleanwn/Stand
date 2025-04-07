#pragma once

#include <fstream>
#include <vector>

#include "TimedText.hpp"

namespace Stand
{
	struct TimedTextCollection
	{
		std::vector<TimedText> entries{};

		[[nodiscard]] static TimedTextCollection fromLRC(std::ifstream& f) noexcept;

		void eraseAdvertisements() noexcept;

		void setStartOffset(time_t offset_millis) noexcept;

		[[nodiscard]] TimedText getTextAtOffset(time_t offset_millis) noexcept;
	};
}
