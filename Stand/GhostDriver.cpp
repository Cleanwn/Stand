#include "GhostDriver.hpp"

#include "AbstractModel.hpp"
#include "CommandToggleRegularSp.hpp"
#include "natives.hpp"
#include "PedType.hpp"
#include "Util.hpp"

namespace Stand
{
	bool GhostDriver::initVehicle(AbstractEntity& veh)
	{
		veh = Util::getVehicle();
		if (veh.isValid())
		{
			if (Vehicle attached_to = ENTITY::GET_ENTITY_ATTACHED_TO(veh))
			{
				if (ENTITY::IS_ENTITY_A_VEHICLE(attached_to))
				{
					veh = AbstractEntity::get(attached_to);
				}
			}
			if (veh.getDriver() != g_player_ped
				&& CommandToggleRegularSp::checkPermissions()
				)
			{
				return true;
			}
		}
		return false;
	}

	void GhostDriver::initDriver(AbstractEntity& outDriver)
	{
		if (!driver.isValid())
		{
			driver = Util::createPed(PED_TYPE_CIVMALE, g_player_ped.getModel(), g_player_ped.getPos(), 0.0f, false);
			ENTITY::SET_ENTITY_HAS_GRAVITY(driver, FALSE);
			ENTITY::SET_ENTITY_COMPLETELY_DISABLE_COLLISION(driver, FALSE, FALSE);
			PED::SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(driver, TRUE);
			ENTITY::SET_ENTITY_VISIBLE(driver, FALSE, FALSE);
			ENTITY::SET_ENTITY_SHOULD_FREEZE_WAITING_ON_COLLISION(driver, FALSE);
		}
		else
		{
			driver.setPos(g_cam_ent.getPos());
		}
		outDriver = driver;
	}

	bool GhostDriver::needsCleanup() const noexcept
	{
		return driver.isValid();
	}

	void GhostDriver::cleanup()
	{
		driver.removeFromPlaneOfExistence();
		driver.reset();

		if (auto veh = Util::getVehicle(); veh.isValid())
		{
			TASK::TASK_VEHICLE_TEMP_ACTION(g_player_ped, veh, 27, 60);
			if (veh.getDriver() == g_player_ped)
			{
				TASK::CLEAR_PED_TASKS(g_player_ped);
				PED::SET_PED_INTO_VEHICLE(g_player_ped, veh, -1);
			}
		}
	}
}
