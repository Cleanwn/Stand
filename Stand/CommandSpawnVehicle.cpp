#include "CommandSpawnVehicle.hpp"

#include <fmt/core.h>

#include "AbstractEntity.hpp"
#include "AbstractModel.hpp"
#include "AbstractPlayer.hpp"
#include "CommandColour.hpp"
#include "CommandListOfSpawnedVehicles.hpp"
#include "CustomDlcMgr.hpp"
#include "FiberPool.hpp"
#include "get_next_arg.hpp"
#include "natives.hpp"
#include "PathFind.hpp"
#include "Script.hpp"
#include "SpawnedVehicleMgr.hpp"
#include "StringUtils.hpp"
#include "using_model.hpp"
#include "Util.hpp"
#include "Vector2Plus.hpp"
#include "VehicleMods.hpp"
#include "vehicle_items.hpp"

namespace Stand
{
	CommandSpawnVehicle::CommandSpawnVehicle(CommandList* const parent, CommandListSelect* spawntuned, CommandToggleNoCorrelation* onfoot_spawnin, CommandToggleNoCorrelation* inveh_spawnin, CommandToggleNoCorrelation* onfoot_airinair, CommandToggleNoCorrelation* inveh_airinair, CommandToggleNoCorrelation* onfoot_spawnfront, CommandToggleNoCorrelation* inveh_spawnfront, CommandToggleNoCorrelation* onfoot_spawndel, CommandToggleNoCorrelation* inveh_spawndel, CommandToggleNoCorrelation* spawngod, CommandToggleNoCorrelation* inveh_spawnvel, CommandToggleNoCorrelation* spawncoloured, CommandColour* spawncolourprimary, CommandColour* spawncoloursecondary, CommandToggleNoCorrelation* onfoot_likepv, CommandToggleNoCorrelation* inveh_likepv, CommandInput* spawn_plate)
		: CommandActionModel(parent, LOC("INPTVEHMDL"), { CMDNAME("spawn") }, LOC("INPTVEHMDL_H"), 0, COMMANDPERM_SPAWN), spawntuned(spawntuned), onfoot_spawnin(onfoot_spawnin), inveh_spawnin(inveh_spawnin), onfoot_airinair(onfoot_airinair), inveh_airinair(inveh_airinair), onfoot_spawnfront(onfoot_spawnfront), inveh_spawnfront(inveh_spawnfront), onfoot_spawndel(onfoot_spawndel), inveh_spawndel(inveh_spawndel), spawngod(spawngod), inveh_spawnvel(inveh_spawnvel), spawncoloured(spawncoloured), spawncolourprimary(spawncolourprimary), spawncoloursecondary(spawncoloursecondary), onfoot_likepv(onfoot_likepv), inveh_likepv(inveh_likepv), spawn_plate(spawn_plate)
	{
		instance = this;
	}

	bool CommandSpawnVehicle::spawnIfValid(const AbstractModel& model, Click& click, const Label& name) const
	{
		if (STREAMING::IS_MODEL_A_VEHICLE(model))
		{
			auto veh = createVehicle(click, model);
			applySpawnTuning(veh);
			return true;
		}
		return false;
	}

	hash_t CommandSpawnVehicle::getInvalidModelMessage() const
	{
		return ATSTRINGHASH("INPTVEHMDL_E");
	}

	AbstractEntity CommandSpawnVehicle::createVehicle(Click& click, hash_t hash) const
	{
		AbstractEntity veh;
		const bool player_ped_is_in_vehicle = g_player_veh.isValid();

		if (CustomDlcMgr::isCustomModel(hash)
			&& !click.isRegularEditionOrSp()
			)
		{
			return veh;
		}

		using_model(hash, [this, click, hash, &veh, player_ped_is_in_vehicle]
		{
			const bool likepv = (player_ped_is_in_vehicle ? inveh_likepv : onfoot_likepv)->m_on;

			float heading = 0.0f;
			v3 pos = click.getEffectiveIssuer().getPos();
			if (likepv)
			{
				pos = PathFind::getClosestNaturalSpawnPosition(hash, pos, heading);
			}
			Vector3 rot;
			if (click.issuedByAndForUser())
			{
				rot = CAM::GET_FINAL_RENDERED_CAM_ROT(2);
				if ((VEHICLE::IS_THIS_MODEL_A_PLANE(hash) || VEHICLE::IS_THIS_MODEL_A_HELI(hash))
					&& ((player_ped_is_in_vehicle ? inveh_airinair : onfoot_airinair)->m_on)
					)
				{
					pos.z += 20.0f;
				}
				else if ((player_ped_is_in_vehicle ? inveh_spawnfront : onfoot_spawnfront)->m_on)
				{
					pos = pos + (rot.toDirNoZ() * 5.0f);
				}
			}
			veh = Util::createVehicle(hash, pos, likepv ? heading : rot.z);
			if (likepv)
			{
				NETWORK::NETWORK_FADE_IN_ENTITY(veh, TRUE, 1);
			}
		});

		SOUP_IF_UNLIKELY (!veh.isValid())
		{
			return veh;
		}

		if (spawngod->m_on)
		{
			veh.setVehicleCanBeDamaged(false);
		}

		if (!spawn_plate->value.empty())
		{
			veh.setPlateText(spawn_plate->value);
		}

		if (click.issuedByAndForUser())
		{
			if (SpawnedVehicleMgr::blips_cmd->m_on)
			{
				SpawnedVehicleMgr::addBlip(veh);
			}
			CommandListOfSpawnedVehicles::makeListItem(veh);
			float speed = -FLT_MAX;
			if (player_ped_is_in_vehicle && inveh_spawnvel->m_on)
			{
				speed = ENTITY::GET_ENTITY_SPEED(g_player_veh);
			}
			else if ((player_ped_is_in_vehicle ? inveh_airinair : onfoot_airinair)->m_on)
			{
				if (VEHICLE::IS_THIS_MODEL_A_PLANE(hash))
				{
					speed = 50.0f;
				}
				else if (VEHICLE::IS_THIS_MODEL_A_HELI(hash))
				{
					veh.startEngine();
				}
			}
			if ((player_ped_is_in_vehicle ? inveh_spawndel : onfoot_spawndel)->m_on)
			{
				auto veh = Util::getVehicle();
				if (veh.isValid() && veh.isOwnerOfVehicleAndDriver())
				{
					veh.removeFromPlaneOfExistence();
				}
			}
			if ((player_ped_is_in_vehicle ? inveh_spawnin : onfoot_spawnin)->m_on)
			{
				PED::SET_PED_INTO_VEHICLE(g_player_ped, veh, -1);
				g_player_veh = veh;
				AbstractEntity::updatePlayerEnt();
			}
			if (speed != -FLT_MAX)
			{
				veh.startEngine();
				VEHICLE::SET_VEHICLE_FORWARD_SPEED(veh, speed);
				if (VEHICLE::GET_VEHICLE_HAS_LANDING_GEAR(veh))
				{
					VEHICLE::CONTROL_LANDING_GEAR(veh, 3);
				}
				if (veh.isVtolOn())
				{
					// We can't disable vtol on same tick as we spawn it, so doing it next tick.
					FiberPool::queueJob([veh]() mutable
					{
						veh.toggleVtolOff();
					});
				}
			}
		}

		return veh;
	}

	void CommandSpawnVehicle::applySpawnTuning(AbstractEntity& veh) const
	{
		if (spawntuned->value == 1)
		{
			veh.applyPerformanceUpgrade();
		}
		else if (spawntuned->value == 2)
		{
			veh.applyPerformanceUpgradeWithSpoiler();
		}
		else if (spawntuned->value == 3)
		{
			veh.fullyUpgrade();
		}
		if (spawncoloured->m_on)
		{
			int r, g, b;
			spawncolourprimary->getRGB(r, g, b);
			VEHICLE::SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(veh, r, g, b);
			spawncoloursecondary->getRGB(r, g, b);
			VEHICLE::SET_VEHICLE_CUSTOM_SECONDARY_COLOUR(veh, r, g, b);
		}
	}
}
