#pragma once

#include <atomic>
#include <functional>
#include <string>

#include "DebugString.hpp"
#include "ErrorCommons.hpp"
#ifdef STAND_DEBUG
#include "Spinlock.hpp"
#endif

namespace Stand
{
	enum class ExceptionContext : uint8_t
	{
		LUA = 1,

		OTHER = 0,
	};

	struct ExceptionData
	{
		_EXCEPTION_POINTERS* const exp;
		const ExceptionContext ctx;
	};

	long handle_exception_spec(void*) noexcept;
	using handle_exception_t = decltype(&handle_exception_spec);

	long handle_caught_exception_spec(ExceptionData*) noexcept;
	using handle_caught_exception_t = decltype(&handle_caught_exception_spec);

	long handle_uncaught_exception_spec(_EXCEPTION_POINTERS*) noexcept;
	using handle_uncaught_exception_t = decltype(&handle_uncaught_exception_spec);
}

extern "C"
{
	// exceptional_helper.asm
	void exceptional_init(Stand::handle_caught_exception_t fpCaughtExp, Stand::handle_uncaught_exception_t fpLogUncaughtExp);
	void disable_exception_handling();
	[[nodiscard]] bool exceptional_has_exp();
	void exceptional_get_exp(void** pOutData, Stand::handle_exception_t* pOutHandler);
	void exceptional_clear_exp();
	long exceptional_on_exception(Stand::ExceptionData* data, Stand::handle_caught_exception_t handler);
	long exceptional_on_caught_exception(Stand::ExceptionData* data);
	long exceptional_on_uncaught_exception(_EXCEPTION_POINTERS* exp);
	void exceptional_set_retval(long retval);
}

#ifdef STAND_DEBUG
#define THREAD_NAME(name) SetThreadDescription(GetCurrentThread(), L##name);
#else
#define THREAD_NAME(name)
#endif

#define __EXCEPTIONAL_CTX(ctx) __except (::exceptional_on_caught_exception(new ::Stand::ExceptionData{GetExceptionInformation(), ::Stand::ExceptionContext::##ctx}))
#define __EXCEPTIONAL() __EXCEPTIONAL_CTX(OTHER)
#define __EXCEPTIONAL_LUA() __EXCEPTIONAL_CTX(LUA)

#define __EXCEPTIONAL_CUSTOM_CTX(handler, ctx) __except (::exceptional_on_exception(new ::Stand::ExceptionData{GetExceptionInformation(), ::Stand::ExceptionContext::##ctx}, handler))
#define __EXCEPTIONAL_CUSTOM(handler) __EXCEPTIONAL_CUSTOM_CTX(handler, OTHER)
#define __EXCEPTIONAL_CUSTOM_LUA(handler) __EXCEPTIONAL_CUSTOM_CTX(handler, LUA)

#define __EXCEPTIONAL_LOG_IF(cond) __except (cond ? ::exceptional_on_caught_exception(new ::Stand::ExceptionData{GetExceptionInformation(), ::Stand::ExceptionContext::OTHER}) : EXCEPTION_EXECUTE_HANDLER)
#define __EXCEPTIONAL_IF(cond) __except (cond ? ::exceptional_on_caught_exception(new ::Stand::ExceptionData{GetExceptionInformation(), ::Stand::ExceptionContext::OTHER}) : EXCEPTION_CONTINUE_SEARCH)

#define EXCEPTIONAL_LOCK(mtx) mtx.lock(); __try {
#define EXCEPTIONAL_TRY_LOCK(mtx) if (mtx.try_lock()) __try {
#define EXCEPTIONAL_UNLOCK(mtx) } __EXCEPTIONAL () { } mtx.unlock();
#define EXCEPTIONAL_QUIET_UNLOCK(mtx) } __except (EXCEPTION_EXECUTE_HANDLER) { } mtx.unlock();

#define EXCEPTIONAL_LOCK_READ(mtx) mtx.lockRead(); __try {
#define EXCEPTIONAL_UNLOCK_READ(mtx) } __EXCEPTIONAL () { } mtx.unlockRead();

#ifdef STAND_DEBUG
#define EXCEPTIONAL_LOCK_WRITE(mtx) mtx.lockWrite(__FUNCTION__); __try {
#else
#define EXCEPTIONAL_LOCK_WRITE(mtx) mtx.lockWrite(); __try {
#endif
#define EXCEPTIONAL_LOCK_WRITE_YIELDING(mtx) while (!mtx.tryLockWrite()) { Script::current()->yield(); } __try {
#define EXCEPTIONAL_UNLOCK_WRITE(mtx) } __EXCEPTIONAL () { } mtx.unlockWrite();

namespace Stand
{
	class Exceptional
	{
	public:
		inline static std::atomic_uint32_t counted_threads = 0;
#ifdef STAND_DEBUG
		inline static Spinlock counted_thread_names_mtx{};
		inline static std::unordered_set<std::string> counted_thread_names{};
#endif

		static void hideThread(HANDLE hThread) noexcept;
		[[nodiscard]] static HANDLE createThread(std::function<void()>&& func) noexcept;
		[[nodiscard]] static HANDLE createExceptionalThread(std::function<void()>&& func) noexcept;
		static void createManagedThread(std::function<void()>&& func) noexcept;
		static void createManagedCountedThread(DebugString name, std::function<void()>&& func) noexcept;
		static void createManagedExceptionalThread(DebugString name, std::function<void()>&& func) noexcept;

		static LPTOP_LEVEL_EXCEPTION_FILTER setUnhandledExceptionHandler();

		inline static volatile bool thread_can_run = true;
		inline static volatile bool thread_is_running = true;
		static void disableExceptionHandling() noexcept;
		static void thread_func() noexcept;

		static void report(const std::string& type, std::string&& message, const ErrorInfo& err = ErrorInfo::fromStack(), void custom_footer_info() = nullptr, ExceptionContext ctx = ExceptionContext::OTHER) noexcept;
		static void report(const std::string& type, std::string&& message, ExceptionData* data, void custom_footer_info() = nullptr) noexcept;
		static void report(const std::string& type, const ErrorInfo& err = ErrorInfo::fromStack()) noexcept;

		static long handleCaughtException(ExceptionData* data) noexcept;
		static long handleUncaughtException(_EXCEPTION_POINTERS* exp) noexcept;

		[[nodiscard]] static std::string getExceptionName(_EXCEPTION_POINTERS* exp) noexcept;
		[[nodiscard]] static std::string getExceptionName(const ExceptionData* data) noexcept;

	private:
		static long handleExceptionInErrorReporting(_EXCEPTION_POINTERS* exp, std::string msg = "Exception in error reporting: ") noexcept;
	};
}
