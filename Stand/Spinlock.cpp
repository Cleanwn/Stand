#include "Spinlock.hpp"

#include <soup/ObfusString.hpp>

#include "Exceptional.hpp"
#include "get_current_time_millis.hpp"

namespace Stand
{
	bool Spinlock::tryLock() noexcept
	{
		uint32_t expected = 0;
		if (owner.compare_exchange_strong(expected, _Thrd_id()))
		{
			return true;
		}
		SOUP_ASSERT(expected != _Thrd_id()); // This isn't a recursive lock.
		return false;
	}

	void Spinlock::lock() noexcept
	{
		time_t start = get_current_time_millis();
		uint32_t expected;
		while (expected = 0, !owner.compare_exchange_weak(expected, _Thrd_id()))
		{
			SOUP_ASSERT(expected != _Thrd_id()); // This isn't a recursive lock.
			if (GET_MILLIS_SINCE(start) > 3000)
			{
				Exceptional::report(soup::ObfusString("Bad Timing"));
				break;
			}
		}
	}

	void Spinlock::unlock() noexcept
	{
		owner = 0;
	}
}
