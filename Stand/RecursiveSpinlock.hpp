#pragma once

#include "SpinlockBase.hpp"

namespace Stand
{
	class RecursiveSpinlock : public SpinlockBase
	{
	private:
		uint8_t recursions = 0;

	public:
		[[nodiscard]] bool tryLock() noexcept;
		void lock() noexcept;
		void unlock() noexcept;
	};
}
