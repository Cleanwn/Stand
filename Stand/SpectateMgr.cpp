#include "SpectateMgr.hpp"

#include "AbstractEntity.hpp"
#include "natives.hpp"

namespace Stand
{
	void SpectateMgr::onTickSpectating(AbstractEntity ped)
	{
		g_overwriting_cam_ent = true;
		g_cam_ped = ped;
		g_cam_veh = ped.getVehicle(false);
		AbstractEntity::updateCamEnt();
	}

	void SpectateMgr::onStopSpectating()
	{
		g_overwriting_cam_ent = false;
	}

	void SpectateMgr::onTickSpectating()
	{
		onTickWithAlteredFocus();
	}

	void SpectateMgr::onTickWithAlteredFocus()
	{
		if (!ENTITY::HAS_COLLISION_LOADED_AROUND_ENTITY(g_player_ped))
		{
			auto ped_pos = g_player_ped.getPos();
			STREAMING::REQUEST_COLLISION_AT_COORD(ped_pos.x, ped_pos.y, ped_pos.z);
		}
	}
}
