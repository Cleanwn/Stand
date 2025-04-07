#pragma once

#define REMOTE_GLOBAL_WRITING true
#define REMOTE_STAT_WRITING false

#if REMOTE_GLOBAL_WRITING

#include <vector>

#include <soup/ObfusString.hpp>

#include "AbstractPlayer.hpp"
#include "Script.hpp"
#include "script_events.hpp"
#include "ScriptGlobal.hpp"
#include "script_handler.hpp"
#include "script_thread.hpp"
#include "tse.hpp"

namespace Stand
{
	// 1.66-2845

	// Don't have remote stat writing with precise values, but we can still hijack other stuff,
	// e.g. "mp0_average_rank_gain_per_hour" to give someone bad sport.
	// ScriptGlobal(2847606).at(98, 3).at(0, 1).set<Hash>(ATSTRINGHASH("MPPLY_OVERALL_BADSPORT"));

	struct RemoteGlobalWriter
	{
		playerbitset_t targets = 0;

		bool prepare(const std::vector<AbstractPlayer>& targets)
		{
			if (auto fm = GtaThread::fromHash(ATSTRINGHASH("freemode")))
			{
				if (auto netcomp = fm->getNetComponent())
				{
					// Ensure we're host and give time for people to realise that
					if (!netcomp->amHost())
					{
						netcomp->takeHost();
						Script::current()->yield(1000);
					}

					// Ensure our OOB global is tampered and give time for people to realise that
					if (ScriptGlobal(2657418 + 41).get<int>() >= 0)
					{
						time_t deadline = get_current_time_millis() + 3000;
						do
						{
							ScriptGlobal(2657418 + 41).set<int>(-soup::rand.t<int>(1, 10));
							Script::current()->yield();
						} while (!IS_DEADLINE_REACHED(deadline));
					}

					for (const auto& p : targets)
					{
						this->targets |= (1 << p);
						if (p != g_player)
						{
							netcomp->giveHostRaw(p.getCNetGamePlayer());
						}
					}

					return true;
				}
			}
			return false;
		}

		void setToMinusOne(ScriptGlobal g)
		{
			constexpr auto origin = ScriptGlobal(2657418 + 41).at(0, 1).offset();

#ifdef STAND_DEBUG
			// array index is unsigned, can't access something before the origin
			if (g.offset() < origin)
			{
				throw 0;
			}
#endif

			uint64_t args[] = {
				Util::i32_to_u64_trickery(-1311894717),
				g_player,
				g.offset() - origin,
				1
			};
			TSE(targets, args);
		}

		void set(ScriptGlobal g, uint64_t value)
		{
			setToMinusOne(ScriptGlobal(2684801 + 4317));

			constexpr auto origin = ScriptGlobal(2684801 + 4317).at(0, 1).offset();

#ifdef STAND_DEBUG
			// array index is unsigned, can't access something before the origin
			if (g.offset() < origin)
			{
				throw 0;
			}
#endif

			uint64_t args[] = {
				Util::i32_to_u64_trickery(-150993329),
				g_player,
				0,
				g.offset() - origin,
				value,
				0
			};
			TSE(targets, args);
		}

		void setStatInt(hash_t hash, int value) = delete;
		void setCharStatInt(const std::string& name, int value) = delete;
	};
}

#endif
