#include "tbStareAtSky.hpp"

#include "Camgr.hpp"
#include "natives.hpp"

namespace Stand
{
	void tbStareAtSky::onTick()
	{
		v3 pos = { 250.0f, 2000.0f, 2690.0f }; // Effective World Centre
		float solar_look_yaw;
		auto hour = float(CLOCK::GET_CLOCK_HOURS())
			+ ((float(CLOCK::GET_CLOCK_MINUTES())
				+ (CLOCK::GET_CLOCK_SECONDS() / 60.0f)
				) / 60.0f);
		if (hour >= 21.0f || hour < 5.0f)
		{
			// Look at moon
			if (hour >= 21.0f)
			{
				hour -= 24.0f;
			}
			solar_look_yaw = 107.0f + (hour * 16.0f);
		}
		else
		{
			// Look at sun
			solar_look_yaw = -90.0f + ((hour - 6.0f) * -12.8f);
		}
		auto rot = v3(0.0f, 0.0f, solar_look_yaw);
		auto unit_vec = rot.toDirNoZ();
		pos.x += (unit_vec.x * 4250.0f);
		pos.y += (unit_vec.y * 6000.0f);
		Camgr::lock_pos(pos);
		Camgr::lock_rot(rot);
		Camgr::lock_fov(60.0f);
	}

	void tbStareAtSky::onDisable()
	{
		Camgr::unlock_pos();
		Camgr::unlock_rot();
		Camgr::unlock_fov();
	}
}
