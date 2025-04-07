#include "PickupManager.hpp"

#include "get_current_time_millis.hpp"
#include "natives.hpp"

namespace Stand
{
	void PickupManager::onTick(time_t now)
	{
		auto i = managed_pickups.begin();
		while (i != managed_pickups.end())
		{
			if (!OBJECT::DOES_PICKUP_EXIST(i->first) || OBJECT::HAS_PICKUP_BEEN_COLLECTED(i->first))
			{
				i = managed_pickups.erase(i);
				continue;
			}
			if (now > i->second)
			{
				Pickup pup = i->first;
				if (NETWORK::NETWORK_HAS_CONTROL_OF_ENTITY(pup))
				{
					OBJECT::DELETE_OBJECT(&pup);
					i = managed_pickups.erase(i);
					continue;
				}
				NETWORK::NETWORK_REQUEST_CONTROL_OF_ENTITY(pup);
			}
			i++;
		}
	}

	void PickupManager::CREATE_AMBIENT_PICKUP(Hash pickupHash, float posX, float posY, float posZ, int flags, int value, Hash modelHash, BOOL p7, BOOL p8)
	{
		Pickup pup = OBJECT::CREATE_AMBIENT_PICKUP(pickupHash, posX, posY, posZ, flags, value, modelHash, p7, p8);
		managed_pickups.emplace(pup, get_current_time_millis() + 10000);
	}
}
