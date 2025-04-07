#include "AttackerMgr.hpp"

#include "AbstractModel.hpp"
#include "BlipUtil.hpp"
#include "CommandPlayerAttackers.hpp"
#include "ChainedVehicleMgr.hpp"
#include "eScriptTaskStatus.hpp"
#include "eTaskCombatPedFlags.hpp"
#include "eTaskThreatResponseFlags.hpp"
#include "get_ground_z.hpp"
#include "Gui.hpp"
#include "PathFind.hpp"
#include "pointers.hpp"
#include "Script.hpp"
#include "Util.hpp"
#include "VehicleType.hpp"

namespace Stand
{
	void AttackerMgr::push(AbstractEntity& attacker, AbstractPlayer victim, Hash weapon, const bool immortality, const Hash vehicle_hash, const bool fill_vehicles, AttackerType type)
	{
		attacker.setCanMigrate(false);

		if (type != AttackerType::NORMAL)
		{
			initAudioBanks(type);
		}

		auto victim_ped = victim.getPed();

		PED::SET_PED_SEEING_RANGE(attacker, 99999.0f);
		PED::SET_PED_HEARING_RANGE(attacker, 99999.0f);
		PED::SET_PED_TARGET_LOSS_RESPONSE(attacker, 2);
		PED::SET_PED_CAN_BE_KNOCKED_OFF_VEHICLE(attacker, 1);
		PED::SET_PED_RELATIONSHIP_GROUP_HASH(attacker, ATSTRINGHASH("ARMY"));
		PED::SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(attacker, true);
		PED::SET_PED_FIRING_PATTERN(attacker, ATSTRINGHASH("FIRING_PATTERN_FULL_AUTO"));

		// CPED_RESET_FLAG_IgnoreCombatManager
		// CPED_RESET_FLAG_AllowPullingPedOntoRoute
		// CPED_RESET_FLAG_UseTighterAvoidanceSettings

		auto blip = HUD::ADD_BLIP_FOR_ENTITY(attacker);
		HUD::SET_BLIP_AS_FRIENDLY(blip, FALSE);
		HUD::SET_BLIP_SCALE(blip, 0.5f);

		ENTITY::SET_ENTITY_LOAD_COLLISION_FLAG(attacker, true, 1);
		TASK::TASK_SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(attacker, true);

		auto vehicle_model = AbstractModel(vehicle_hash);
		auto attacker_model = attacker.getModel();

		if (vehicle_model.isPlane())
		{
			int task;
			TASK::OPEN_SEQUENCE_TASK(&task);
			TASK::TASK_COMBAT_PED(attacker, victim_ped, eTaskCombatPedFlags::COMBAT_PED_PREVENT_CHANGING_TARGET, eTaskThreatResponseFlags::TASK_THREAT_RESPONSE_CAN_FIGHT_ARMED_PEDS_WHEN_NOT_ARMED);
			TASK::SET_SEQUENCE_TO_REPEAT(task, TRUE);
			TASK::CLOSE_SEQUENCE_TASK(task);
			TASK::TASK_PERFORM_SEQUENCE(attacker, task);
			TASK::CLEAR_SEQUENCE_TASK(&task);
		}
		else
		{
			TASK::TASK_COMBAT_PED(attacker, victim_ped, eTaskCombatPedFlags::COMBAT_PED_PREVENT_CHANGING_TARGET, eTaskThreatResponseFlags::TASK_THREAT_RESPONSE_CAN_FIGHT_ARMED_PEDS_WHEN_NOT_ARMED);
		}

		if (immortality)
		{
			attacker.godmodeEnable();
		}

		if (vehicle_hash != 0)
		{
			ENTITY::SET_ENTITY_VISIBLE(attacker, FALSE, FALSE);
		}

		if (attacker_model.isAnimal())
		{
			weapon = ATSTRINGHASH("WEAPON_ANIMAL");
		}

		auto last_ped = attackers.empty() ? AbstractEntity::invalid() : attackers.back().attacker;
		auto vehicle = ChainedVehicleMgr::processVehicle(attacker, vehicle_model, last_ped, immortality, fill_vehicles, false);

		if (vehicle.isValid())
		{
			customiseVehicle(vehicle, type);
		}

		customiseAttacker(attacker, type);

		attacker.giveWeapons({ weapon }, true);
		attackers.emplace_back(AttackerGroup{ attacker, victim, vehicle, type });
	}

	void AttackerMgr::clear(compactplayer_t p)
	{
		for (auto i = attackers.begin(); i != attackers.end(); )
		{
			AttackerGroup& g = *i;

			if (g.victim == p)
			{
				g.pop();
				i = attackers.erase(i);
			}
			else
			{
				++i;
			}
		}
	}

	void AttackerMgr::onTick()
	{
		for (auto i = attackers.begin(); i != attackers.end(); )
		{
			const auto& player = i->victim;
			auto& attacker = i->attacker;

			if (!player.exists()
				|| !attacker.isValid()
				|| g_gui.isUnloadPending()
				)
			{
				i->pop();
				i = attackers.erase(i);
			}
			else if (attacker.isDead())
			{
				if (auto blip = HUD::GET_BLIP_FROM_ENTITY(attacker))
				{
					BlipUtil::remove(blip);
				}
				++i;
			}
			else
			{
				auto& vehicle = i->vehicle;
				AbstractModel vehicle_model = vehicle.getModel();

				if (vehicle.exists()) // isValid is insufficient: https://www.guilded.gg/stand/groups/x3ZgB10D/channels/47eb8ea9-21be-4642-8916-b062b4f9cac0/forums/1910839838
				{
					onTickVehicle(*i);
				}

				onTickAttacker(*i);

				if (auto ped = player.getPed(); ped.exists())
				{
					applyTasks(attacker, vehicle, ped, i->type);

					PED::SET_RELATIONSHIP_BETWEEN_GROUPS(5, ATSTRINGHASH("ARMY"), PED::GET_PED_RELATIONSHIP_GROUP_HASH(ped));
				}

				if (PED::GET_PED_RELATIONSHIP_GROUP_HASH(attacker) != ATSTRINGHASH("ARMY"))
				{
					PED::SET_PED_RELATIONSHIP_GROUP_HASH(attacker, ATSTRINGHASH("ARMY"));
				}

				if (auto accuracy = getAccuracyForPlayer(player); (accuracy / 100.0f) != PED::GET_COMBAT_FLOAT(attacker, 6))
				{
					PED::SET_PED_ACCURACY(attacker, accuracy);
				}

				++i;
			}
		}
	}

	bool AttackerMgr::canCreateAttackers(compactplayer_t p)
	{
		return attackers.size() <= 40 && !locked_players.contains(p);
	}

	int AttackerMgr::getAccuracyForPlayer(const AbstractPlayer& player)
	{
		if (player.exists())
		{
			SOUP_IF_LIKELY (auto cmd = player.getCommandAsList())
			{
				SOUP_IF_LIKELY (auto trolling = cmd->resolveChildByMenuName(LOC("TROLL")))
				{
					SOUP_IF_LIKELY (auto attackers = trolling->as<CommandList>()->resolveChildByMenuName(LOC("ATTKRS")))
					{
						SOUP_IF_LIKELY (auto a = attackers->as<CommandPlayerAttackers>(); a->accuracy)
						{
							a->accuracy->onTickInGameViewport();
							return a->accuracy->value;
						}
					}
				}
			}
		}

		return 100;
	}

	void AttackerMgr::customiseVehicle(AbstractEntity& veh, AttackerType type)
	{
		switch (type)
		{
		case AttackerType::NORMAL:
			veh.fullyUpgrade();
			break;

		case AttackerType::PHANTOM:
			{
				veh.setHeadlightsVariation(10);

				AUDIO::SET_VEHICLE_RADIO_ENABLED(veh, FALSE);

				VEHICLE::SET_VEHICLE_DOORS_LOCKED_FOR_ALL_PLAYERS(veh, TRUE);
				VEHICLE::SET_DONT_ALLOW_PLAYER_TO_ENTER_VEHICLE_IF_LOCKED_FOR_PLAYER(veh, TRUE);
				VEHICLE::SET_VEHICLE_EXTRA(veh, 12, TRUE);
				VEHICLE::SET_VEHICLE_COLOURS(veh, 32, 32);
				VEHICLE::SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(veh, 40, 0, 0);
				VEHICLE::SET_VEHICLE_CUSTOM_SECONDARY_COLOUR(veh, 40, 0, 0);
				VEHICLE::SET_VEHICLE_EXTRA_COLOUR_5(veh, 1);
				VEHICLE::SET_VEHICLE_EXTRA_COLOUR_6(veh, 6);
				VEHICLE::SET_VEHICLE_WINDOW_TINT(veh, 3);
				VEHICLE::SET_VEHICLE_NEON_COLOUR(veh, 255, 255, 255);
				VEHICLE::SET_VEHICLE_NUMBER_PLATE_TEXT(veh, soup::ObfusString("SPOOKY").c_str());
				VEHICLE::SET_VEHICLE_LIGHTS(veh, 2);
				VEHICLE::SET_VEHICLE_LIGHT_MULTIPLIER(veh, 15.0f);

				AUDIO::REQUEST_SCRIPT_AUDIO_BANK("DLC_TUNER/DLC_Tuner_Phantom_Car", TRUE, Util::get_session_players_bitflag());
				AUDIO::PLAY_SOUND_FROM_ENTITY(-1, "Spawn_In_Game", veh, "DLC_Tuner_Halloween_Phantom_Car_Sounds", TRUE, FALSE);
			}
			break;
		}
	}

	void AttackerMgr::customiseAttacker(AbstractEntity& ped, AttackerType type)
	{
		switch (type)
		{
		case AttackerType::NORMAL:
			break;

		case AttackerType::PHANTOM:
			{
				AUDIO::DISABLE_PED_PAIN_AUDIO(ped, TRUE);
				AUDIO::STOP_PED_SPEAKING_SYNCED(ped, TRUE);
				ENTITY::SET_ENTITY_ALPHA(ped, 0, FALSE);
				ENTITY::SET_ENTITY_VISIBLE(ped, FALSE, FALSE);
			}
			break;
		}
	}

	void AttackerMgr::applyTasks(AbstractEntity& attacker, AbstractEntity& veh, AbstractEntity& victim, AttackerType type)
	{
		const bool is_driver = veh.getDriver() == attacker;

		switch (type)
		{
		case AttackerType::NORMAL:
			{
				const auto model = veh.getModel();

				if (is_driver)
				{
					if (model.isPlane() // This is only done specifically for planes because they are the only vehicle type which exhibits increased aggression from it.
						|| TASK::GET_SCRIPT_TASK_STATUS(attacker, ATSTRINGHASH("script_task_vehicle_mission")) != eScriptTaskStatus::PERFORMING_TASK
						)
					{
						switch (model.getVehicleType())
						{
						case VEHICLE_TYPE_BOAT:
						case VEHICLE_TYPE_SUBMARINE:
							TASK::TASK_BOAT_MISSION(attacker, veh, 0, victim, 0.0f, 0.0f, 0.0f, 4, 200.0f, 786469, -1.0f, 7);
							break;

						case VEHICLE_TYPE_HELI:
							TASK::TASK_HELI_MISSION(attacker, veh, 0, victim, 0.0f, 0.0f, 0.0f, 9, 9999.0f, 40.0f, -1.0f, 9999, 20, -1.0f, 0);
							break;

						case VEHICLE_TYPE_PLANE:
							TASK::TASK_PLANE_MISSION(attacker, veh, 0, victim, 0.0f, 0.0f, 0.0f, 6, -1.0f, -1.0f, -1.0f, 300.0f, 100.0f, TRUE);
							break;

						default:
							VEHICLE::MODIFY_VEHICLE_TOP_SPEED(veh, 40.0f);
							PED::SET_DRIVER_AGGRESSIVENESS(attacker, 1.0f);
							TASK::TASK_VEHICLE_MISSION_PED_TARGET(attacker, veh, victim, 22, 9999.0f, 0, -1.0f, -1.0f, TRUE);
						}
					}
				}

				if (!(model.isPlane() && is_driver) && !victim.isDead()) // Giving the task to plane pilots will interrupt cannon fire. We want to give the task to passengers instead.
				{
					if (TASK::GET_SCRIPT_TASK_STATUS(attacker, ATSTRINGHASH("script_task_combat")) != eScriptTaskStatus::PERFORMING_TASK)
					{
						TASK::TASK_COMBAT_PED(attacker, victim, eTaskCombatPedFlags::COMBAT_PED_PREVENT_CHANGING_TARGET, eTaskThreatResponseFlags::TASK_THREAT_RESPONSE_CAN_FIGHT_ARMED_PEDS_WHEN_NOT_ARMED);
					}
				}
			}
			break;

		case AttackerType::PHANTOM:
			{
				if (is_driver)
				{
					if (TASK::GET_SCRIPT_TASK_STATUS(attacker, ATSTRINGHASH("script_task_vehicle_mission")) != eScriptTaskStatus::PERFORMING_TASK)
					{
						TASK::TASK_VEHICLE_MISSION_PED_TARGET(attacker, veh, victim, 6, 100.0f, 0, 0.0f, 0.0f, TRUE);
					}
				}
				else if (TASK::GET_SCRIPT_TASK_STATUS(attacker, ATSTRINGHASH("script_task_combat")) != eScriptTaskStatus::PERFORMING_TASK)
				{
					TASK::TASK_COMBAT_PED(attacker, victim, eTaskCombatPedFlags::COMBAT_PED_PREVENT_CHANGING_TARGET, eTaskThreatResponseFlags::TASK_THREAT_RESPONSE_CAN_FIGHT_ARMED_PEDS_WHEN_NOT_ARMED);
				}
			}
			break;
		}
	}

	void AttackerMgr::onTickVehicle(AttackerGroup& group)
	{
		auto& vehicle = group.vehicle;
		auto& attacker = group.attacker;

		if (!vehicle.isOwner())
		{
			NETWORK::NETWORK_REQUEST_CONTROL_OF_ENTITY(vehicle);
		}

		switch (group.type)
		{
		case AttackerType::NORMAL:
			{
				if (attacker.getVehicle() == vehicle
					&& !ENTITY::IS_ENTITY_VISIBLE(attacker)
					)
				{
					ENTITY::SET_ENTITY_VISIBLE(attacker, TRUE, FALSE);
				}

				if (!ENTITY::IS_ENTITY_VISIBLE(vehicle)) // 1/100 aircraft randomy go invisible when getting their task. R* bug afaik.
				{
					ENTITY::SET_ENTITY_VISIBLE(vehicle, TRUE, FALSE);
				}

				if (vehicle.getModel().isPlane())
				{
					group.processPlanePositionUpdate();

					if (!VEHICLE::_ARE_MISSILE_BAYS_DEPLOYED(vehicle))
					{
						VEHICLE::_SET_DEPLOY_MISSILE_BAYS(vehicle, TRUE); // Needs to be called per-tick.
					}
				}
			}
			break;

		case AttackerType::PHANTOM:
			{
				int& sound_id = group.sound_id;
				int& ptfx_handle = group.ptfx_handle;

				if (ptfx_handle == -1)
				{
					if (STREAMING::HAS_NAMED_PTFX_ASSET_LOADED("scr_tn_phantom"))
					{
						GRAPHICS::USE_PARTICLE_FX_ASSET("scr_tn_phantom");
						ptfx_handle = GRAPHICS::START_NETWORKED_PARTICLE_FX_LOOPED_ON_ENTITY("scr_tn_phantom_flames", group.vehicle, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 180.0f, 1.0f, FALSE, TRUE, FALSE, 1065353216, 1065353216, 1065353216, 0);
					}
					else
					{
						STREAMING::REQUEST_NAMED_PTFX_ASSET("scr_tn_phantom");
					}
				}
				else if (!GRAPHICS::DOES_PARTICLE_FX_LOOPED_EXIST(ptfx_handle))
				{
					ptfx_handle = -1;
				}

				if (sound_id == -1)
				{
					sound_id = AUDIO::GET_SOUND_ID();
					AUDIO::REQUEST_SCRIPT_AUDIO_BANK("DLC_TUNER/DLC_Tuner_Phantom_Car", TRUE, Util::get_session_players_bitflag());
					AUDIO::PLAY_SOUND_FROM_ENTITY(sound_id, "Flames_Loop", group.vehicle, "DLC_Tuner_Halloween_Phantom_Car_Sounds", TRUE, FALSE);
				}
				else if (AUDIO::HAS_SOUND_FINISHED(sound_id))
				{
					AUDIO::RELEASE_SOUND_ID(sound_id);
					sound_id = -1;
				}
			}
			break;
		}
	}

	void AttackerMgr::onTickAttacker(AttackerGroup& group)
	{
		auto& attacker = group.attacker;

		if (!attacker.isOwner())
		{
			NETWORK::NETWORK_REQUEST_CONTROL_OF_ENTITY(attacker);
		}

		switch (group.type)
		{
		case AttackerType::PHANTOM:
			{
				if (ENTITY::IS_ENTITY_VISIBLE(attacker))
				{
					ENTITY::SET_ENTITY_VISIBLE(attacker, FALSE, FALSE);
				}
			}
			break;
		}
	}

	void AttackerMgr::initAudioBanks(AttackerType type)
	{
		if (*pointers::is_session_started)
		{
			CGameScriptId dummy_script{};
			dummy_script.m_unk = 0;
			dummy_script.m_hash = 0;
			dummy_script.m_name[0] = 0;
			dummy_script.m_timestamp = 0;
			dummy_script.instance_id = -1;
			dummy_script.m_position_hash = 0;

			for (auto hash : getAudioBankDependencies(type))
			{
				pointers::CAudioBankRequestEvent_Trigger(hash, &dummy_script, true, Util::get_session_players_bitflag());
			}
		}
	}

	std::vector<Hash> AttackerMgr::getAudioBankDependencies(AttackerType type) noexcept
	{
		switch (type)
		{
		case AttackerType::PHANTOM:
			return { ATSTRINGHASH("DLC_TUNER/DLC_Tuner_Phantom_Car") };
		}

		SOUP_ASSERT(false);
	}

	bool AttackerMgr::isEntityAttacker(AbstractEntity& entity)
	{
		for (auto& g : attackers)
		{
			if (g.attacker == entity || g.vehicle == entity)
			{
				return true;
			}
		}

		return false;
	}

	void AttackerMgr::lockAttackerCreation(compactplayer_t p)
	{
		locked_players.emplace(p);
	}

	void AttackerMgr::unlockAttackerCreation(compactplayer_t p)
	{
		SOUP_IF_LIKELY (auto i = locked_players.find(p); i != locked_players.end())
		{
			locked_players.erase(i);
		}
	}
}
