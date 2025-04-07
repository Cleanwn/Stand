#pragma once

#include <atomic>
#include <queue>

#include <soup/macros.hpp> // MULTI_MACRO
#include <soup/Pattern.hpp>
#include <soup/pattern_macros.hpp>
#include <soup/Range.hpp>

#include "Codename.hpp"
#include "ColoadMgr.hpp"
#include "Exceptional.hpp"
#include "nullsub.hpp"
#include "Spinlock.hpp"
#include "StringUtils.hpp"

namespace Stand
{
#define BATCH_ADD(name, sig, func) \
{ \
	SIG_INST(sig); \
	batch.add(name, g_gta_module, std::move(sig_inst), func); \
}

#define BATCH_ADD_OPTIONAL_3(name, sig, func) \
{ \
	SIG_INST(sig); \
	batch.add_optional(name, g_gta_module, std::move(sig_inst), func); \
}
#define BATCH_ADD_OPTIONAL_4(name, sig, succ_func, fail_func) \
{ \
	SIG_INST(sig); \
	batch.add_optional(name, g_gta_module, std::move(sig_inst), succ_func, fail_func); \
}
#define BATCH_ADD_OPTIONAL(...) MULTI_MACRO(BATCH_ADD_OPTIONAL, __VA_ARGS__)

inline Spinlock existing_hooks_mtx = {};
inline std::string existing_hooks = {};

#define CHECK_EXISTING_HOOK_IMPL(name) \
if (sig_inst != p.as<const void*>()) \
{ \
	EXCEPTIONAL_LOCK(existing_hooks_mtx) \
	StringUtils::list_append(existing_hooks, Codename(name).toString()); \
	EXCEPTIONAL_UNLOCK(existing_hooks_mtx) \
}

#define CHECK_EXISTING_HOOK(name, head_sig) \
{ \
	SIMPLE_SIG_INST(head_sig); \
	CHECK_EXISTING_HOOK_IMPL(name); \
}

#define CHECK_EXISTING_HOOK_WONT_HOOK(name, head_sig) \
{ \
	SIMPLE_SIG_INST(head_sig); \
	CHECK_EXISTING_HOOK_IMPL(name); \
	Codename cn(name); \
	ColoadMgr::existing_hook_checks.emplace_front(ExistingHookCheck{ cn, p.as<void*>(), sig_inst.getVec() }); \
}

#define BATCH_ADD_FUNC_IMPL(name, pname, head_sig, body_sig, batch_func, store_func) \
{ \
	SIG_INST(body_sig); \
	batch.batch_func(name, g_gta_module, std::move(sig_inst), [](soup::Pointer p) \
	{ \
		p = p.sub(5); \
		CHECK_EXISTING_HOOK(name, head_sig); \
		store_func(pname); \
	}); \
}

#define BATCH_ADD_FUNC(name, pname, head_sig, body_sig) BATCH_ADD_FUNC_IMPL(name, pname, head_sig, body_sig, add, STORE_POINTER)
#define BATCH_ADD_OPTIONAL_FUNC(name, pname, head_sig, body_sig) BATCH_ADD_FUNC_IMPL(name, pname, head_sig, body_sig, add_optional, STORE_POINTER)

#define STORE_POINTER(name) pointers::name = p.as<decltype(pointers::name)>();

	class PatternBatch;

	using pattern_callback_t = void(*)(soup::Pointer);
	using pattern_fail_callback_t = void(*)(PatternBatch&);

	class PatternBatch
	{
	public:
		struct Entry
		{
			std::string name;
			soup::Range range;
			soup::Pattern pattern;
			pattern_callback_t callback;
			pattern_fail_callback_t fail_callback;

			explicit Entry(std::string&& name, soup::Range&& range, soup::Pattern&& pattern, pattern_callback_t callback, pattern_fail_callback_t fail_callback)
				: name(std::move(name)), range(std::move(range)), pattern(std::move(pattern)), callback(callback), fail_callback(fail_callback)
			{
			}
		};

		std::unordered_map<std::string, uintptr_t>* cache = nullptr;

#ifdef STAND_DEBUG
		std::unordered_set<std::string> entry_names{};
#endif
		std::queue<Entry> entries;

		std::string error_message = {};
		std::atomic_uint16_t cache_utilisation = 0;
		std::string allowed_fails = {};

		explicit PatternBatch() = default;
		explicit PatternBatch(std::unordered_map<std::string, uintptr_t>* cache);

		~PatternBatch() noexcept = default;

		void add(const Codename& name, soup::Range&& range, soup::Pattern&& pattern, pattern_callback_t callback);
		void add_optional(const Codename& name, soup::Range&& range, soup::Pattern&& pattern, pattern_callback_t callback, pattern_fail_callback_t fail_callback = nullsub<>);
	private:
		void add_impl(const Codename& name, soup::Range&& range, soup::Pattern&& pattern, pattern_callback_t callback, pattern_fail_callback_t fail_callback);
		
	public:
		void run();
		void runSingleThread();
	};
}
