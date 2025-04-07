#include "VehicleCustomisationIo.hpp"

#include "AbstractEntity.hpp"
#include "CommandSpawnVehicle.hpp"
#include "natives.hpp"
#include "Util.hpp"
#include "VehicleEntityCustomisationIo.hpp"

namespace Stand
{
	bool VehicleCustomisationIo::isPropertyApplicable(AbstractEntity& veh, int idx)
	{
		if (isVehicleMod(idx))
		{
			return veh.isUpgradeApplicable(translatePropertyToVehicleMod(idx));
		}
		switch (idx)
		{
		case ROOF_LIVERY:
			return VEHICLE::GET_VEHICLE_LIVERY2_COUNT(veh) > 0;

		case EXTRA_1:
		case EXTRA_2:
		case EXTRA_3:
		case EXTRA_4:
		case EXTRA_5:
		case EXTRA_6:
		case EXTRA_7:
		case EXTRA_8:
		case EXTRA_9:
		case EXTRA_10:
		case EXTRA_11:
		case EXTRA_12:
		case EXTRA_13:
		case EXTRA_14:
		case EXTRA_15:
			return VEHICLE::DOES_EXTRA_EXIST(veh, ((idx - EXTRA_1) + 1));
		}
		return true;
	}

	void VehicleCustomisationIo::applyTo(AbstractEntity& veh)
	{
		VehicleEntityCustomisationIo ent_cust{ veh };
		copyTo(ent_cust);
	}

	AbstractEntity VehicleCustomisationIo::createEntityNow(bool networked)
	{
		AbstractEntity veh = Util::createVehicle(read(MODEL), {}, 0.0f, networked);
		applyTo(veh);
		return veh;
	}

	void VehicleCustomisationIo::spawnClone(Click& click)
	{
		AbstractEntity veh = CommandSpawnVehicle::instance->createVehicle(click, read(MODEL));
		applyTo(veh);
	}
}
