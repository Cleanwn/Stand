#pragma once

#include <atomic>

namespace Stand
{
	class SpinlockBase
	{
	protected:
		std::atomic_uint32_t owner = 0;

	public:
		[[nodiscard]] bool isLocked() const noexcept;
		[[nodiscard]] bool isLockedByThisThread() const noexcept;
	};
}
