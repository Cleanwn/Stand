#include "RecursiveSpinlock.hpp"

#include <soup/ObfusString.hpp>

#include "Exceptional.hpp"
#include "get_current_time_millis.hpp"

namespace Stand
{
	bool RecursiveSpinlock::tryLock() noexcept
	{
		uint32_t expected = 0;
		SOUP_IF_LIKELY (owner.compare_exchange_strong(expected, _Thrd_id()))
		{
			return true;
		}
		if (expected == _Thrd_id())
		{
			++recursions;
			return true;
		}
		return false;
	}

	void RecursiveSpinlock::lock() noexcept
	{
		time_t start = get_current_time_millis();
		uint32_t expected;
		while (expected = 0, !owner.compare_exchange_weak(expected, _Thrd_id()))
		{
			if (expected == _Thrd_id())
			{
				++recursions;
				break;
			}
			if (GET_MILLIS_SINCE(start) > 3000)
			{
				Exceptional::report(soup::ObfusString("Bad Timing"));
				break;
			}
		}
	}

	void RecursiveSpinlock::unlock() noexcept
	{
		if (recursions == 0)
		{
			owner = 0;
		}
		else
		{
			--recursions;
		}
	}
}
