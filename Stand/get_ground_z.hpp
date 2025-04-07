#pragma once

#include <utility>

namespace Stand
{
	[[nodiscard]] extern bool is_in_water_bounds(float x, float y);
	[[nodiscard]] extern float get_ground_z(float x, float y, float z_hint = 1000.0f);
	[[nodiscard]] extern float get_ground_z_water(float x, float y);
	[[nodiscard]] extern float get_ground_z_water_inbounds(float x, float y);
	[[nodiscard]] extern float get_ground_z_shapetest(float x, float y, float z_hint = 1000.0f);
}
