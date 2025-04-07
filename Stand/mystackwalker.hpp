#pragma once

#include <StackWalker.h>

#include <optional>
#include <vector>

#include "ErrorCommons.hpp"

#define GENERATE_STACK_TRACE ::Stand::MyStackWalker::getInstance()->generateStackTrace(::Stand::ErrorInfo::fromStack());

#define SSW_CALLSTACK_ONLY true

namespace Stand
{
	class MyStackWalker : public StackWalker
	{
	protected:
		using StackWalker::StackWalker;
	public:
		MyStackWalker();
		~MyStackWalker();

		static std::shared_ptr<MyStackWalker> getInstance();

		void OnCallstackEntry(CallstackEntryType type, CallstackEntry& entry) final;

		[[nodiscard]] static constexpr DWORD getMaxNameLength()
		{
			return STACKWALK_MAX_NAMELEN;
		}

		[[nodiscard]] HANDLE getProcessHandle() const;

	private:
		inline static std::vector<void*> callstack_entries;
	public:
#if !SSW_CALLSTACK_ONLY
		inline static bool callback_may_be_corrupted = false;
#endif

		void generateStackTrace(const ErrorInfo& err, const std::string& prefix = {});
	private:
		static void diveIntoStack(void** sp, std::optional<void*>&& opt_ip, const std::string& prefix);
	};

	inline std::shared_ptr<MyStackWalker> g_mystackwalker_inst{};
}
