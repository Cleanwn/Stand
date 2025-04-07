#include "get_ground_z.hpp"

#include "AbstractEntity.hpp"
#include "natives.hpp"
#include "pointers.hpp"
#include "WaterQuad.hpp"

namespace Stand
{
	bool is_in_water_bounds(float x, float y)
	{
		return x >= float(*pointers::world_x_min) && x <= float(*pointers::world_x_max)
			&& y >= float(*pointers::world_y_min) && y <= float(*pointers::world_y_max);
	}

	float get_ground_z(float x, float y, float z_hint)
	{
		if (!is_in_water_bounds(x, y))
		{
			return 0.0f;
		}
		const float ground_z_shapetest = get_ground_z_shapetest(x, y, z_hint);
		if (ground_z_shapetest == -200.0f)
		{
			return -200.0f;
		}
		return std::max(
			get_ground_z_water_inbounds(x, y),
			ground_z_shapetest
		);
    }

	float get_ground_z_water(float x, float y)
	{
		if (is_in_water_bounds(x, y))
		{
			return get_ground_z_water_inbounds(x, y);
		}
		return 0.0f;
	}

	float get_ground_z_water_inbounds(float x, float y)
	{
		if (WaterQuad* water_quad = WaterQuad::at(x, y))
		{
			return water_quad->z;
		}
		return -200.0f;
	}

#define SCRIPT_INCLUDE_MOVER		1
#define SCRIPT_INCLUDE_VEHICLE		2
#define SCRIPT_INCLUDE_PED			4
#define SCRIPT_INCLUDE_RAGDOLL		8
#define SCRIPT_INCLUDE_OBJECT		16
#define SCRIPT_INCLUDE_PICKUP		32
#define SCRIPT_INCLUDE_GLASS		64
#define SCRIPT_INCLUDE_RIVER		128
#define SCRIPT_INCLUDE_FOLIAGE		256
#define SCRIPT_INCLUDE_ALL			-1 // 511

#define SCRIPT_SHAPETEST_OPTION_IGNORE_GLASS		1
#define SCRIPT_SHAPETEST_OPTION_IGNORE_SEE_THROUGH	2
#define SCRIPT_SHAPETEST_OPTION_IGNORE_NO_COLLISION	4
#define SCRIPT_SHAPETEST_OPTION_DEFAULT				SCRIPT_SHAPETEST_OPTION_IGNORE_GLASS | SCRIPT_SHAPETEST_OPTION_IGNORE_SEE_THROUGH | SCRIPT_SHAPETEST_OPTION_IGNORE_NO_COLLISION

	float get_ground_z_shapetest(float x, float y, float z_hint)
	{
		const int shapetest_handle = SHAPETEST::START_EXPENSIVE_SYNCHRONOUS_SHAPE_TEST_LOS_PROBE(x, y, z_hint, x, y, -200.0f, SCRIPT_INCLUDE_ALL, g_player_ped, SCRIPT_SHAPETEST_OPTION_IGNORE_NO_COLLISION);
		BOOL hit;
		Vector3 endCoords;
		Vector3 surfaceNormal;
		Entity entityHit;
		SHAPETEST::GET_SHAPE_TEST_RESULT(shapetest_handle, &hit, &endCoords, &surfaceNormal, &entityHit);
		if (hit)
		{
			return endCoords.z;
		}
		return -200.0f;
	}
}
