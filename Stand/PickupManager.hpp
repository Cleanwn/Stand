#pragma once

#include <unordered_map>

#include <windows.h>

#include "natives_decl.hpp"
#include "typedecl.hpp"

namespace Stand
{
	class PickupManager
	{
	private:
		static inline std::unordered_map<Pickup, time_t> managed_pickups{};

	public:
		static void onTick(time_t now);

		static void CREATE_AMBIENT_PICKUP(Hash pickupHash, float posX, float posY, float posZ, int flags, int value, Hash modelHash, BOOL p7, BOOL p8);
	};
}
