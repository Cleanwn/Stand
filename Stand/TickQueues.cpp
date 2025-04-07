#include "TickQueues.hpp"

#include "AbstractEntity.hpp"
#include "AbstractPlayer.hpp"
#include "fwPool.hpp"
#include "get_current_time_millis.hpp"
#include "natives.hpp"
#include "pointers.hpp"

namespace Stand
{
	[[nodiscard]] static bool can_send_ne() noexcept
	{
		return *pointers::rage_netGameEvent_ms_pPool
			&& (*pointers::rage_netGameEvent_ms_pPool)->GetNoOfFreeSpaces() > 150
			;
	}

	void TickQueues::onTick()
	{
		for (auto i = arm_queue.begin(); i != arm_queue.end() && can_send_ne(); )
		{
			AbstractPlayer p(i->first);
			if (p.exists() && p.hasPed())
			{
				WEAPON::GIVE_DELAYED_WEAPON_TO_PED(p.getPed(), i->second.front(), 9999, false);
				i->second.pop();
			}
			if (i->second.empty())
			{
				i = arm_queue.erase(i);
			}
			else
			{
				++i;
			}
		}

		if (!disarm_queue.empty())
		{
			while (can_send_ne())
			{
				std::unordered_map<uint64_t, std::pair<bool, time_t>>::iterator disarm;
				long long disarm_age = -1;
				for (auto i = disarm_queue.begin(); i != disarm_queue.end(); i++)
				{
					if (i->second.first && (i->second.second < disarm_age || disarm_age == -1))
					{
						disarm = i;
						disarm_age = i->second.second;
					}
				}
				if (disarm_age == -1)
				{
					break;
				}
				AbstractPlayer p(disarm->first >> 32);
				if (p.exists() && p.hasPed())
				{
					const Hash weapon_hash = (disarm->first & 0xFFFFFFFF);
					//g_logger.log(fmt::format("Removing {} from {}", weapon_hash, p.get_name()));
					WEAPON::REMOVE_WEAPON_FROM_PED(p.getPed(), weapon_hash);
					disarm->second.first = false;
					disarm->second.second = get_current_time_millis() + 10000;
				}
				else
				{
					disarm_queue.erase(disarm);
				}
			}
		}
	}
}
