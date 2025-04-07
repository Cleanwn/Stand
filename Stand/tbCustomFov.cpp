#include "tbCustomFov.hpp"

#include "AbstractEntity.hpp"
#include "Camgr.hpp"
#include "CommandSlider.hpp"
#include "natives.hpp"

namespace Stand
{
	void tbCustomFov::onTick()
	{
		if (!Camgr::user_fov_allowed)
		{
			had_custom_fov = false;
			return;
		}

		int custom_fov_this_tick = -1;

		if (g_player_ped.isAiming())
		{
			Hash weapon;
			WEAPON::GET_CURRENT_PED_WEAPON(g_player_ped, &weapon, 1);
			if (WEAPON::GET_WEAPONTYPE_GROUP(weapon) == -1212426201)
			{
				custom_fov_this_tick = sniping->value;
			}
			else
			{
				custom_fov_this_tick = aiming->value;
			}
		}
		else if (g_player_veh.isValid())
		{
			if (CAM::GET_CAM_VIEW_MODE_FOR_CONTEXT(CAM::GET_CAM_ACTIVE_VIEW_MODE_CONTEXT()) == 4)
			{
				custom_fov_this_tick = fp_in_vehicle->value;
			}
			else
			{
				custom_fov_this_tick = tp_in_vehicle->value;
			}
		}
		else
		{
			if (CAM::GET_CAM_VIEW_MODE_FOR_CONTEXT(CAM::GET_CAM_ACTIVE_VIEW_MODE_CONTEXT()) == 4)
			{
				custom_fov_this_tick = fp_on_foot->value;
			}
			else
			{
				custom_fov_this_tick = tp_on_foot->value;
			}
		}

		if (custom_fov_this_tick >= 0)
		{
			had_custom_fov = true;
			Camgr::lock_fov((float)custom_fov_this_tick);
		}
		else
		{
			if (had_custom_fov)
			{
				had_custom_fov = false;
				Camgr::unlock_fov();
			}
		}
	}

	void tbCustomFov::onDisable()
	{
		Camgr::unlock_fov();
	}
}
