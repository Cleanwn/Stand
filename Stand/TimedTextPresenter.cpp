#include "TimedTextPresenter.hpp"

#include "get_current_time_millis.hpp"

namespace Stand
{
	void TimedTextPresenter::startPresenting(TimedTextCollection&& c) noexcept
	{
		presentation = std::move(c);
		started_at = get_current_time_millis();
	}

	TimedText TimedTextPresenter::getCurrentText() noexcept
	{
		return presentation.getTextAtOffset(GET_MILLIS_SINCE(started_at));
	}
}
