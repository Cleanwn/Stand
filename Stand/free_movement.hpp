#pragma once

#include "vector.hpp"

namespace Stand
{
	extern bool levitate_is_on;

	extern bool free_movement(v3& pos, const v3& rot, const float speed = 1.0f, const float sprint_speed = 5.0f);
}
