#include "tbPositionSelection.hpp"

#include "AbstractEntity.hpp"
#include "Camgr.hpp"
#include "natives.hpp"

namespace Stand
{
	void tbPositionSelection::onEnable()
	{
		tbToggleableBehaviourScript::onEnable();
		Camgr::freecam_focus_allowed = false;
	}

	void tbPositionSelection::onDisable()
	{
		Camgr::freecam_focus_allowed = true;
		Camgr::unlock_focus();
	}

	void tbPositionSelection::onTick()
	{
		auto cam_pos = CAM::GET_FINAL_RENDERED_CAM_COORD();
		auto cam_rot = CAM::GET_FINAL_RENDERED_CAM_ROT(2);
		auto cam_vec = cam_rot.toDir() * 50.0f;
		auto cam_target = cam_pos;
		v3 closest_chunk{};
		float closest_chunk_dist = FLT_MAX;
		for (uint8_t i = 0; i < (8500 / 50); ++i)
		{
			cam_target += cam_vec;
			auto chunk_z = PATHFIND::GET_APPROX_HEIGHT_FOR_POINT(cam_target.x, cam_target.y);
			v3 chunk{ cam_target.x, cam_target.y, chunk_z };
			auto chunk_dist = chunk.distance(cam_target);
			if (chunk_dist < closest_chunk_dist)
			{
				closest_chunk = chunk;
				closest_chunk_dist = chunk_dist;
			}
		}

		const int shapetest_handle = SHAPETEST::START_EXPENSIVE_SYNCHRONOUS_SHAPE_TEST_LOS_PROBE(cam_pos.x, cam_pos.y, cam_pos.z, cam_target.x, cam_target.y, cam_target.z, -1, g_player_ped, 0);
		BOOL hit;
		Vector3 endCoords;
		Vector3 surfaceNormal;
		Entity entityHit = 0;
		SHAPETEST::GET_SHAPE_TEST_RESULT(shapetest_handle, &hit, &endCoords, &surfaceNormal, &entityHit);
		if (hit)
		{
			pos = endCoords;
			Camgr::unlock_focus();
		}
		else
		{
			Camgr::lock_focus(closest_chunk);
		}
	}
}
