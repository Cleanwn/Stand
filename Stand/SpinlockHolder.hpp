#pragma once

#include "Spinlock.hpp"

namespace Stand
{
	template <typename T>
	struct SpinlockHolder
	{
		T& mtx;
		bool locked = false;

		SpinlockHolder(T& mtx)
			: mtx(mtx)
		{
		}

		~SpinlockHolder() noexcept
		{
			if (locked)
			{
				mtx.unlock();
			}
		}

		[[nodiscard]] bool tryLock() noexcept
		{
			locked = mtx.tryLock();
			return locked;
		}
	};
}
