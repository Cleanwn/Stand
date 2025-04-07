#include "TranscendentVehicle.hpp"

#include "natives.hpp"
#include "VehicleEntityCustomisationIo.hpp"

namespace Stand
{
	void TranscendentVehicle::saveFromPlayerState()
	{
		if (g_player_veh.isFullyUsable()
			&& !g_player_veh.getPos().isNull()
			)
		{
			save(g_player_veh);
		}
		else
		{
			active = false;
		}
	}

	void TranscendentVehicle::save(AbstractEntity& veh)
	{
		active = true;
		ent = veh;
		VehicleEntityCustomisationIo cust_io{ veh };
		cust_io.copyTo(cust_data);
		pos = veh.getPos();
		rot = veh.getRot();
		extra.save(veh);
	}

	void TranscendentVehicle::recover()
	{
		ent = cust_data.createEntityNow(true);
		if (ent.isValid())
		{
			ent.setPos(pos);
			ent.setRot(rot);
			ent.startEngine();
			PED::SET_PED_INTO_VEHICLE(g_player_ped, ent, -1);
			extra.restore(ent);
			CAM::SET_THIRD_PERSON_CAM_RELATIVE_PITCH_LIMITS_THIS_UPDATE(0.0f, 0.0f);
			CAM::SET_THIRD_PERSON_CAM_RELATIVE_HEADING_LIMITS_THIS_UPDATE(0.0f, 0.0f);
		}
	}

	void TranscendentVehicle::removeEntity()
	{
		ent.removeFromPlaneOfExistence();
		ent.reset();
	}
}
