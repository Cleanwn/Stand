#include "ChainedVehicleMgr.hpp"

#include "eCombatAttributes.hpp"
#include "ExecCtx.hpp"
#include "get_ground_z.hpp"
#include "PathFind.hpp"
#include "Script.hpp"
#include "Util.hpp"

namespace Stand
{
	v3 ChainedVehicleMgr::randomiseAbove(const v3& pos)
	{
		int attempts = 0;
		Vector3 result = Vector3(pos);

		STREAMING::REQUEST_COLLISION_AT_COORD(result.x, result.y, result.z);
		if (float ground_z = get_ground_z(result.x, result.y); ground_z != -200.0f)
		{
			result.z = ground_z;
		}

		result.z += (float)soup::rand(40, 100);

		while (++attempts < 500)
		{
			auto x = (float)soup::rand(20, 50);
			auto y = (float)soup::rand(20, 50);
			result.x += soup::rand.coinflip() ? x : -x;
			result.y += soup::rand.coinflip() ? y : -y;

			if (!MISC::IS_POSITION_OCCUPIED(result.x, result.y, result.z, 20.0f, TRUE, TRUE, TRUE, FALSE, FALSE, 0, FALSE))
			{
				break;
			}
		}

		return result;
	}

	AbstractEntity ChainedVehicleMgr::processVehicle(AbstractEntity& ped, AbstractModel& vehicle_model, AbstractEntity& last_ped, const bool immortality, const bool fill_vehicles, const bool can_leave_vehicles)
	{
		SOUP_ASSERT(ExecCtx::get().tc == TC_SCRIPT_YIELDABLE);

		if (const auto hash = (hash_t)vehicle_model)
		{
			float hdn = 0.0f;

			if (last_ped.exists() && fill_vehicles)
			{
				auto last_vehicle = last_ped.getVehicle();

				if (last_vehicle.isValid())
				{
					if (VEHICLE::ARE_ANY_VEHICLE_SEATS_FREE(last_vehicle))
					{
						PED::SET_PED_INTO_VEHICLE(ped, last_vehicle, -2);
						return last_vehicle;
					}
				}
			}

			STREAMING::REQUEST_MODEL(hash);
			while (!STREAMING::HAS_MODEL_LOADED(hash))
			{
				Script::current()->yield();
			}

			auto pos = ped.getPos();

			if (!vehicle_model.isAirborne())
			{
				pos = PathFind::getClosestNaturalSpawnPosition(vehicle_model, ped.getPos(), hdn, 3, false);
			}
			else
			{
				pos = randomiseAbove(pos);
			}

			auto veh = Util::createVehicle(hash, pos, hdn);

			veh.startEngine();
			veh.setCanMigrate(false);

			if (immortality)
			{
				veh.makeIndestructible();
			}

			if (vehicle_model.isAirborne())
			{
				auto vpos = veh.getPos();
				vpos.z += 50.0f;
				veh.setPos(vpos);

				if (vehicle_model.isPlane())
				{
					VEHICLE::CONTROL_LANDING_GEAR(veh, 3);
					VEHICLE::SET_VEHICLE_FORWARD_SPEED(veh, 60.0f);
					VEHICLE::SET_VEHICLE_FORCE_AFTERBURNER(veh, TRUE);
				}
			}

			PED::SET_PED_INTO_VEHICLE(ped, veh, -1);

			ped.setCombatAttr(eCombatAttributes::CA_USE_VEHICLE, true);
			ped.setCombatAttr(eCombatAttributes::CA_ALWAYS_FIGHT, true);
			ped.setCombatAttr(eCombatAttributes::CA_LEAVE_VEHICLES, can_leave_vehicles);

			Script::current()->yield(200);

			return veh;
		}

		return AbstractEntity::invalid();
	}
}