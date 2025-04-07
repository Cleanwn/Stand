#pragma once

#include "AbstractEntity.hpp"
#include "CustomisationData.hpp"
#include "PositionStateVehData.hpp"
#include "VehicleCustomisationIo.hpp"

namespace Stand
{
	struct TranscendentVehicle
	{
		inline static bool active = false;
		inline static AbstractEntity ent;
		inline static CustomisationData<VehicleCustomisationIo> cust_data;
		inline static v3 pos;
		inline static v3 rot;
		inline static PositionStateVehData extra;

		static void saveFromPlayerState();
		static void save(AbstractEntity& veh);
		static void recover();
		static void removeEntity();
	};
}
