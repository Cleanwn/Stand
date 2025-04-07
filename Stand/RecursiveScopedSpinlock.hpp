#pragma once

#include <atomic>

#include "FixedVector.hpp"

#define REPORT_LONG_WRITE_LOCK false

namespace Stand
{
	class RecursiveScopedSpinlock
	{
	private:
		FixedVector<uint32_t, 10> readers{};
		std::atomic<uint32_t> writer = 0;
		uint8_t write_recursions = 0;
#if REPORT_LONG_WRITE_LOCK
		time_t time_locked;
#endif

	public:
		[[nodiscard]] bool isReadLocked() const noexcept;
		[[nodiscard]] bool isReadLockedByThisThread() const noexcept;
		[[nodiscard]] bool isWriteLocked() const noexcept;
		[[nodiscard]] bool isWriteLockedByThisThread() const noexcept;
		[[nodiscard]] bool isLockedByThisThread() const noexcept;

		void lockRead() noexcept;
	private:
		void lockReadInner() noexcept;
		void extendReadLock() noexcept;
	public:
		void unlockRead() noexcept;

		void lockWrite() noexcept;
#ifdef STAND_DEBUG
		void lockWrite(const char* reason) noexcept;
#endif
	private:
		void lockWriteInner() noexcept;
	public:
		[[nodiscard]] bool tryLockWrite() noexcept;
		void unlockWrite() noexcept;
	};
}
