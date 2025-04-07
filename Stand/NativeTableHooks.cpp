#include "NativeTableHooks.hpp"

#include <unordered_map>

#include <soup/Exception.hpp>

#include "atStringHash.hpp"
#include "Exceptional.hpp"
#include "NativeCallContext.hpp"
#include "native_tables.hpp"
#include "pointers.hpp"
#include "ScriptMgr.hpp"
#include "script_program.hpp"
#ifdef STAND_DEBUG
#include "Util.hpp"
#endif

#define NTH_GC_NOTIFY false

#if NTH_GC_NOTIFY
#include "ThreadContext.hpp"
#include "Util.hpp"
#endif

namespace Stand
{
	struct EntrypointOverride
	{
		XoredNativeHash native;
		rage::scrNativeHandler detour;

		[[nodiscard]] uint32_t getNativeTableIndex(rage::scrProgram* prgm) const
		{
			if (auto e = native_tables.find(native); e != native_tables.end())
			{
				if (auto f = e->second.find(prgm->m_name_hash); f != e->second.end())
				{
					return f->second;
				}
				return -1;
			}
			return prgm->getNativeTableIndex(NativeTableHooks::og(native));
		}
	};

	struct ScriptExtra
	{
		bool unseen = false;
		bool up_to_date = false;
		std::unordered_map<XoredNativeHash, uint16_t> detoured_natives{};

		void apply(rage::scrProgram* prgm, const EntrypointOverride& eo)
		{
			if (auto e = detoured_natives.find(eo.native); e != detoured_natives.end())
			{
				prgm->m_native_entrypoints[e->second] = eo.detour;
			}
			else if (auto idx = eo.getNativeTableIndex(prgm); idx != -1)
			{
				prgm->m_native_entrypoints[idx] = eo.detour;
				detoured_natives.emplace(eo.native, idx);
			}
		}

		void restore(rage::scrProgram* prgm, XoredNativeHash hash, rage::scrNativeHandler handler)
		{
			if (auto e = detoured_natives.find(hash); e != detoured_natives.end())
			{
				prgm->m_native_entrypoints[e->second] = handler;
				detoured_natives.erase(e);
			}
		}

		void restoreAll(rage::scrProgram* prgm)
		{
			for (const auto& dn : detoured_natives)
			{
				prgm->m_native_entrypoints[dn.second] = NativeTableHooks::og(dn.first);
			}
			detoured_natives.clear();
		}
	};

	static std::unordered_map<hash_t, ScriptExtra> script_extras;
	static std::vector<EntrypointOverride> global_overrides;
	static std::unordered_map<hash_t, std::vector<EntrypointOverride>> local_overrides;

	[[nodiscard]] static ScriptExtra& getScriptExtra(hash_t hash)
	{
		if (auto e = script_extras.find(hash); e != script_extras.end())
		{
			return e->second;
		}
		return script_extras.emplace(hash, ScriptExtra{}).first->second;
	}

	void NativeTableHooks::onTick()
	{
		size_t num_unseen_extras = 0;

		// Flag all scripts as unseen
		for (auto& e : script_extras)
		{
			e.second.unseen = true;
			++num_unseen_extras;
		}

		// Process running scripts
		for (uint64_t i = 0; i != pointers::script_programs_size; ++i)
		{
			if (pointers::script_programs[i] == nullptr)
			{
				continue;
			}

			auto& extra = getScriptExtra(pointers::script_programs[i]->m_name_hash);
			if (extra.unseen)
			{
				extra.unseen = false;
				--num_unseen_extras;
			}
			if (extra.up_to_date)
			{
				continue;
			}
			extra.up_to_date = true;

			for (const auto& eo : global_overrides)
			{
				extra.apply(pointers::script_programs[i], eo);
			}

			if (auto e = local_overrides.find(pointers::script_programs[i]->m_name_hash); e != local_overrides.end())
			{
				for (const auto& eo : e->second)
				{
					extra.apply(pointers::script_programs[i], eo);
				}
			}
		}

		if (num_unseen_extras != 0)
		{
#if NTH_GC_NOTIFY
			size_t gcd = 0;
#endif
			for (auto i = script_extras.begin(); i != script_extras.end(); )
			{
				if (i->second.unseen)
				{
					i = script_extras.erase(i);
#if NTH_GC_NOTIFY
					++gcd;
#endif
				}
				else
				{
					++i;
				}
			}
#if NTH_GC_NOTIFY
			Util::toast(fmt::format("[NTH-Tick] Cleaned up {} stale script extras.", gcd), TC_SCRIPT_NOYIELD);
#endif
		}
	}

	rage::scrNativeHandler NativeTableHooks::og(XoredNativeHash native)
	{
		return g_script_mgr.handler_map.at(native);
	}

	void NativeTableHooks::createNativeEntrypointOverride(XoredNativeHash native, rage::scrNativeHandler entrypoint_override)
	{
		if (!NativeCallContext::canInvoke(native))
		{
			return;
		}

		// Check if the native is already hooked
		for (auto& eo : global_overrides)
		{
			if (eo.native == native)
			{
				throw soup::Exception("Precondition unmet");
			}
		}

		g_script_mgr.mapEntrypointReverse(entrypoint_override, native.getHash());
		
		// Add override
		global_overrides.emplace_back(EntrypointOverride{ native, entrypoint_override });

		// Flag all scripts for update
		for (auto& e : script_extras)
		{
			e.second.up_to_date = false;
		}
	}

	void NativeTableHooks::createNativeEntrypointOverride(hash_t script, XoredNativeHash native, rage::scrNativeHandler entrypoint_override)
	{
		if (!NativeCallContext::canInvoke(native))
		{
			return;
		}

		g_script_mgr.mapEntrypointReverse(entrypoint_override, native.getHash());

		// Add override
		if (auto e = local_overrides.find(script); e != local_overrides.end())
		{
			e->second.emplace_back(EntrypointOverride{ native, entrypoint_override });
		}
		else
		{
			local_overrides.emplace(script, std::vector<EntrypointOverride>{ EntrypointOverride{ native, entrypoint_override } });
		}

		// Flag script for update if running
		if (auto e = script_extras.find(script); e != script_extras.end())
		{
			e->second.up_to_date = false;
		}
	}

	void NativeTableHooks::removeNativeEntrypointOverride(XoredNativeHash native)
	{
		if (NativeCallContext::canInvoke(native))
		{
			// Using "og" here simply gets the entrypoint for our native invoker,
			// which is fine as long as that's the game's native handler and not a custom one.
			removeNativeEntrypointOverride(native, og(native));
		}
	}

	void NativeTableHooks::removeNativeEntrypointOverride(XoredNativeHash native, rage::scrNativeHandler og_entrypoint)
	{
		// Restore native table entries
#if NTH_GC_NOTIFY
		size_t gcd = 0;
#endif
		for (auto i = script_extras.begin(); i != script_extras.end(); )
		{
			SOUP_IF_LIKELY (auto prgm = rage::scrProgram::fromHash(i->first))
			{
				i->second.restore(prgm, native, og_entrypoint);
				++i;
			}
			else
			{
				i = script_extras.erase(i);
#if NTH_GC_NOTIFY
				++gcd;
#endif
			}
		}
#if NTH_GC_NOTIFY
		Util::toast(fmt::format("[NTH-Remove] Cleaned up {} stale script extras.", gcd), TC_SCRIPT_NOYIELD);
#endif
		
		// Remove override, error if override was not registered
		bool removed_override = false;
		for (auto i = global_overrides.begin(); i != global_overrides.end(); ++i)
		{
			if (i->native == native)
			{
				global_overrides.erase(i);
				removed_override = true;
				break;
			}
		}
		if (!removed_override)
		{
			Exceptional::report("Precondition unmet");
		}
	}

	void NativeTableHooks::removeNativeEntrypointOverride(hash_t script, XoredNativeHash native)
	{
		if (!NativeCallContext::canInvoke(native))
		{
			return;
		}

		// Restore native table entry
		if (auto e = script_extras.find(script); e != script_extras.end())
		{
			if (auto prgm = rage::scrProgram::fromHash(script))
			{
				e->second.restore(prgm, native, og(native));
			}
			else
			{
				script_extras.erase(e);
			}
		}

		// Remove override, error if override was not registered
		bool removed_override = false;
		if (auto e = local_overrides.find(script); e != local_overrides.end())
		{
			for (auto i = e->second.begin(); i != e->second.end(); ++i)
			{
				if (i->native == native)
				{
					e->second.erase(i);
					removed_override = true;
					break;
				}
			}
			if (e->second.empty())
			{
				local_overrides.erase(e);
			}
		}
		if (!removed_override)
		{
			Exceptional::report("Precondition unmet");
		}
	}

	void NativeTableHooks::forceCleanup()
	{
		for (auto& e : script_extras)
		{
			if (!e.second.detoured_natives.empty())
			{
				if (auto prgm = rage::scrProgram::fromHash(e.first))
				{
#ifdef STAND_DEBUG
					Util::toast("Native table hook was not removed naturally, force cleanup had to take care of this!", TOAST_ALL);
#endif
					e.second.restoreAll(prgm);
				}
			}
		}
		script_extras.clear();
		global_overrides.clear();
		local_overrides.clear();
	}
}
