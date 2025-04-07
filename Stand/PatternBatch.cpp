#include "PatternBatch.hpp"

#include <thread>

#include <soup/ObfusString.hpp>

#include "Codename.hpp"
#include "Exceptional.hpp"
#include "main.hpp"
#include "RecursiveSpinlock.hpp"
#include "StringUtils.hpp"

namespace Stand
{
	static RecursiveSpinlock mtx{};
	static PatternBatch* inst = nullptr;

	PatternBatch::PatternBatch(std::unordered_map<std::string, uintptr_t>* cache)
		: cache(cache)
	{
	}

	void PatternBatch::add(const Codename& name, soup::Range&& range, soup::Pattern&& pattern, pattern_callback_t callback)
	{
		add_impl(name, std::move(range), std::move(pattern), callback, nullptr);
	}

	void PatternBatch::add_optional(const Codename& name, soup::Range&& range, soup::Pattern&& pattern, pattern_callback_t callback, pattern_fail_callback_t fail_callback)
	{
		add_impl(name, std::move(range), std::move(pattern), callback, fail_callback);
	}

	void PatternBatch::add_impl(const Codename& name, soup::Range&& range, soup::Pattern&& pattern, pattern_callback_t callback, pattern_fail_callback_t fail_callback)
	{
		EXCEPTIONAL_LOCK(mtx)
#ifdef STAND_DEBUG
		if (entry_names.contains(name.toString()))
		{
			Exceptional::report("Duplicate pattern name");
		}
		entry_names.emplace(name.toString());
#endif
		entries.emplace(name.toString(), std::move(range), std::move(pattern), callback, fail_callback);
		EXCEPTIONAL_UNLOCK(mtx)
	}

	void threadPatternScan()
	{
		do
		{
			mtx.lock();
			if (inst->entries.empty())
			{
				mtx.unlock();
				return;
			}
			PatternBatch::Entry entry = std::move(inst->entries.front());
			inst->entries.pop();
			bool from_cache = false;
			soup::Pointer result;
			if (inst->cache)
			{
				auto cached = inst->cache->find(entry.name);
				if (cached != inst->cache->end())
				{
					result = (entry.range.base.as<uintptr_t>() + cached->second);
					mtx.unlock();
					if (result.isInRange(entry.range)
						&& soup::Range::pattern_matches(result.as<uint8_t*>(), entry.pattern.bytes.data(), entry.pattern.bytes.size())
						)
					{
						from_cache = true;
						inst->cache_utilisation++;
					}
					else
					{
						result = nullptr;
					}
				}
				else
				{
					mtx.unlock();
				}
			}
			else
			{
				mtx.unlock();
			}
			if (!result)
			{
				result = entry.range.scan(entry.pattern);
			}
			if (result)
			{
				entry.callback(result);
				if (!from_cache)
				{
					uintptr_t offset = (result.as<uintptr_t>() - entry.range.base.as<uintptr_t>());
					if (inst->cache)
					{
						EXCEPTIONAL_LOCK(mtx)
						auto cache_i = inst->cache->find(entry.name);
						if (cache_i == inst->cache->end())
						{
							inst->cache->emplace(entry.name, std::move(offset));
						}
						else
						{
							cache_i->second = std::move(offset);
						}
						EXCEPTIONAL_UNLOCK(mtx)
					}
				}
			}
			else if (entry.fail_callback != nullptr)
			{
				EXCEPTIONAL_LOCK(mtx)
				StringUtils::list_append(inst->allowed_fails, entry.name);
				entry.fail_callback(*inst);
				EXCEPTIONAL_UNLOCK(mtx)
			}
			else
			{
				EXCEPTIONAL_LOCK(mtx)
				if (inst->error_message.empty())
				{
					inst->error_message = soup::ObfusString("Failed to find pattern(s): ").str().append(entry.name);
				}
				else
				{
					inst->error_message.append(", ").append(entry.name);
				}
				EXCEPTIONAL_UNLOCK(mtx)
			}
		} while (true);
	}

	void PatternBatch::run()
	{
		error_message.clear();
		inst = this;

		unsigned int threads_to_spin_up = std::thread::hardware_concurrency();
		if (threads_to_spin_up > entries.size())
		{
			threads_to_spin_up = (unsigned int)entries.size();
		}

		std::vector<HANDLE> threads;
		for (unsigned int i = 0; i < threads_to_spin_up; i++)
		{
			threads.emplace_back(Exceptional::createExceptionalThread(&threadPatternScan));
		}

		WaitForMultipleObjects((DWORD)threads.size(), &threads.at(0), TRUE, INFINITE);

		for (auto& thread : threads)
		{
			CloseHandle(thread);
		}
	}

	void PatternBatch::runSingleThread()
	{
		error_message.clear();
		inst = this;

		threadPatternScan();
	}
}
