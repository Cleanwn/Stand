#pragma once

#include "AbstractEntity.hpp"
#include "AbstractModel.hpp"

namespace Stand
{
	struct ChainedVehicleMgr
	{
		static v3 randomiseAbove(const v3& pos);
		static AbstractEntity processVehicle(AbstractEntity& ped, AbstractModel& vehicle_model, AbstractEntity& last_ped, const bool immortality, const bool fill_vehicles, const bool can_leave_vehicles);
	};
}