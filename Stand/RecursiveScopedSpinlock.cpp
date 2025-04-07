#include "RecursiveScopedSpinlock.hpp"

#include <soup/ObfusString.hpp>

#include "Exceptional.hpp"
#include "get_current_time_millis.hpp"

#define LOG_WRITE_LOCK_FAIL_DUE_TO_READERS false
#define RSNYM_LOG_WRITE false
#define PROFILE_LOCK_WRITE false

#if LOG_WRITE_LOCK_FAIL_DUE_TO_READERS || RSNYM_LOG_WRITE
#include "FileLogger.hpp"
#endif

#if PROFILE_LOCK_WRITE
#include "TimedCall.hpp"
#endif

// This saves us a lot of headaches with Gui::recursivelyApplyState.
#define KEEP_READERS_UPGRADED true

namespace Stand
{
	bool RecursiveScopedSpinlock::isReadLocked() const noexcept
	{
		return !readers.empty();
	}

	bool RecursiveScopedSpinlock::isReadLockedByThisThread() const noexcept
	{
		return readers.contains(_Thrd_id());
	}

	bool RecursiveScopedSpinlock::isWriteLocked() const noexcept
	{
		return writer != 0;
	}

	bool RecursiveScopedSpinlock::isWriteLockedByThisThread() const noexcept
	{
		return /*isWriteLocked() &&*/ writer == _Thrd_id();
	}

	bool RecursiveScopedSpinlock::isLockedByThisThread() const noexcept
	{
		return isWriteLockedByThisThread() || isReadLockedByThisThread();
	}

	void RecursiveScopedSpinlock::lockRead() noexcept
	{
		if (isReadLockedByThisThread())
		{
			extendReadLock();
		}
		else
		{
			lockReadInner();
		}
	}

	void RecursiveScopedSpinlock::lockReadInner() noexcept
	{
#if KEEP_READERS_UPGRADED
		if (isWriteLockedByThisThread())
		{
			++write_recursions;
		}
#endif

		auto tid = _Thrd_id();

		readers.emplace(tid);

		if (isWriteLocked() && writer != tid)
		{
			time_t start = get_current_time_millis();
			do
			{
				if (GET_MILLIS_SINCE(start) > 3000)
				{
					Exceptional::report(soup::ObfusString("Bad Timing"));
					break;
				}
			} while (isWriteLocked());
		}
	}

	void RecursiveScopedSpinlock::extendReadLock() noexcept
	{
		readers.emplace(_Thrd_id());
	}

	void RecursiveScopedSpinlock::unlockRead() noexcept
	{
		readers.erase(_Thrd_id());

#if KEEP_READERS_UPGRADED
		if (!isReadLockedByThisThread()
			&& isWriteLockedByThisThread()
			)
		{
			unlockWrite();
		}
#endif
	}

	void RecursiveScopedSpinlock::lockWrite() noexcept
	{
		if (!isWriteLockedByThisThread())
		{
#if RSNYM_LOG_WRITE
			g_logger.log(soup::ObfusString("lockWrite"));
#endif
			lockWriteInner();
		}
		else
		{
#if RSNYM_LOG_WRITE
			g_logger.log(soup::ObfusString("lockWrite (already locked)"));
#endif
			++write_recursions;
		}
	}

#ifdef STAND_DEBUG
	void RecursiveScopedSpinlock::lockWrite(const char* reason) noexcept
	{
#if PROFILE_LOCK_WRITE
		TimedCall::once(fmt::format("Obtain write lock for {}", reason), [this]
		{
#endif
			lockWrite();
#if PROFILE_LOCK_WRITE
		});
#endif
	}
#endif

	void RecursiveScopedSpinlock::lockWriteInner() noexcept
	{
#ifdef STAND_DEBUG
		SOUP_ASSERT(write_recursions == 0);
#endif

		time_t start = get_current_time_millis();
	_retry:

		// Set writer
		{
			for (_Thrd_id_t expected; expected = 0, !writer.compare_exchange_weak(expected, _Thrd_id()); )
			{
#ifdef STAND_DEBUG
				if (expected == _Thrd_id())
				{
					// This should've already been dealt with in lockWrite and race condition within same thread is impossible.
					SOUP_ASSERT_UNREACHABLE;

					++write_recursions;
					return;
				}
#endif
				if (GET_MILLIS_SINCE(start) > 3000)
				{
					Exceptional::report(soup::ObfusString("Bad Timing"));
					return;
				}
			}
		}

		// Yield if there are still readers active
		if (readers.countExcept(_Thrd_id()) != 0)
		{
			if (GET_MILLIS_SINCE(start) > 3000)
			{
				Exceptional::report(soup::ObfusString("Bad Timing"));
				return;
			}
			writer = 0;
			YieldProcessor();
			goto _retry;
		}

#if REPORT_LONG_WRITE_LOCK
		time_locked = get_current_time_millis();
#endif
	}

	bool RecursiveScopedSpinlock::tryLockWrite() noexcept
	{
		// Bail if other threads are reading.
		if (readers.countExcept(_Thrd_id()) != 0)
		{
#if RSNYM_LOG_WRITE
			g_logger.log(soup::ObfusString("tryLockWrite - false"));
#endif
			return false;
		}

		// Atomically check if writer is 0 and set to this thread if so.
		_Thrd_id_t expected = 0;
		if (writer.compare_exchange_strong(expected, _Thrd_id()))
		{
			// Relinquish write lock again if a new reader came to be.
			if (readers.countExcept(_Thrd_id()) != 0)
			{
				writer = 0;
#if RSNYM_LOG_WRITE
				g_logger.log(soup::ObfusString("tryLockWrite - false"));
#endif
				return false;
			}

#ifdef STAND_DEBUG
			SOUP_ASSERT(write_recursions == 0);
#endif
#if REPORT_LONG_WRITE_LOCK
			time_locked = get_current_time_millis();
#endif
#if RSNYM_LOG_WRITE
			g_logger.log(soup::ObfusString("tryLockWrite - true"));
#endif
			return true;
		}

		// Someone is already writing, check if it is this thread.
		if (expected == _Thrd_id())
		{
			++write_recursions;
#if RSNYM_LOG_WRITE
			g_logger.log(soup::ObfusString("tryLockWrite - true"));
#endif
			return true;
		}

#if RSNYM_LOG_WRITE
		g_logger.log(soup::ObfusString("tryLockWrite - false"));
#endif
		return false;
	}

	void RecursiveScopedSpinlock::unlockWrite() noexcept
	{
		SOUP_IF_UNLIKELY (!isWriteLockedByThisThread())
		{
			// Assuming that we hit "Bad Timing" when locking and just went past, so nothing to do.
			return;
		}

		if (write_recursions == 0)
		{
#if KEEP_READERS_UPGRADED
			if (isReadLockedByThisThread())
			{
				//Exceptional::report("Reader remains a writer");
				return;
			}
#endif
			writer = 0;
#if RSNYM_LOG_WRITE
			g_logger.log(soup::ObfusString("unlockWrite"));
#endif
#if REPORT_LONG_WRITE_LOCK
			if (GET_MILLIS_SINCE(time_locked) > 3000)
			{
				Exceptional::report(soup::ObfusString("Possible cause of 'Bad Timing' errors"));
			}
#endif
		}
		else
		{
			--write_recursions;
#if RSNYM_LOG_WRITE
			g_logger.log(soup::ObfusString("unlockWrite (still locked)"));
#endif
		}
	}
}
