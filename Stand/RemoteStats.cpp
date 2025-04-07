#include "RemoteStats.hpp"

#include <soup/UniqueList.hpp>

#include "Exceptional.hpp"
#include "Spinlock.hpp"
#include "pointers.hpp"

namespace Stand
{
	static Spinlock mtx;
	static soup::UniqueList<RemoteStats> instances;

	RemoteStats::RemoteStats(int64_t rid)
		: rid(rid)
	{
		results.Reset(statIds, COUNT(statIds), 1, &record, sizeof(record));

		record.m_Values[LAST_CHAR].m_Type = rage::RL_PROFILESTATS_TYPE_INVALID;

		rage::rlGamerHandle gamers[1] = { rid };
		pointers::rage_rlProfileStats_ReadStatsByGamer(0, gamers, COUNT(gamers), &results, &status);
	}

	RemoteStats* RemoteStats::get(int64_t rid)
	{
		RemoteStats* s = nullptr;
		EXCEPTIONAL_LOCK(mtx)
		for (auto& inst : instances)
		{
			if (inst.rid == rid)
			{
				s = &inst;
				break;
			}
		}
		if (s == nullptr)
		{
			s = instances.emplace_back(new RemoteStats(rid));
		}
		EXCEPTIONAL_UNLOCK(mtx)
		return s;
	}

	bool RemoteStats::tryFree(RemoteStats*& s)
	{
		if (s->isPending())
		{
			return false;
		}
		EXCEPTIONAL_LOCK(mtx)
		instances.erase(s);
		EXCEPTIONAL_UNLOCK(mtx)
		s = nullptr;
		return true;
	}

	bool RemoteStats::shutdown()
	{
		bool done = true;
		EXCEPTIONAL_LOCK(mtx)
		for (auto i = instances.begin(); i != instances.end(); )
		{
			if (i->isPending())
			{
				done = false;
				break;
			}
			i = instances.erase(i);
		}
		EXCEPTIONAL_UNLOCK(mtx)
		return done;
	}
}
