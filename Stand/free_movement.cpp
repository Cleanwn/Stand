#include "free_movement.hpp"

#include <soup/math.hpp> // lerp

#include "gta_input.hpp"
#include "input.hpp"
#include "natives.hpp"

namespace Stand
{
	bool levitate_is_on = false;

	bool free_movement(v3& pos, const v3& rot, const float speed, const float sprint_speed)
	{
		v3 moveVec;
		moveVec -= (rot.toDir() * Input::getControlNormal(INPUT_MOVE_UD));
		moveVec -= (v3(0, 0, rot.z + 90.0f).toDir() * Input::getControlNormal(INPUT_MOVE_LR));
		if (Input::isAscDescAvailable())
		{
#if !CONTROLLER_ANALOG_ASCDESC
			if (Input::isController())
			{
				if (Input::isControlPressed(Input::getAscend()))
				{
					moveVec.z += 1.0f;
				}
				if (Input::isControlPressed(Input::getDescend()))
				{
					moveVec.z -= 1.0f;
				}
			}
			else
#endif
			{
				moveVec.z += Input::getControlNormal(Input::getAscend(), Input::getDescend());
			}
		}
		if (moveVec.isNull())
		{
			return false;
		}
		moveVec *= speed;
		if (Input::isFreeSprintAvailable())
		{
			moveVec *= soup::lerp(1.0f, sprint_speed, Input::getControlNormal(Input::getFreeSprint()));
		}
		pos += moveVec;
		return true;
	}
}
