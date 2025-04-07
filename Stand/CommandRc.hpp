#pragma once

#include "CommandToggleRegularSp.hpp"

#include "Camgr.hpp"
#include "get_current_time_millis.hpp"
#include "GhostDriver.hpp"
#include "gta_input.hpp"
#include "gta_vehicle.hpp"
#include "input.hpp"
#include "is_session.hpp"
#include "lang.hpp"
#include "Util.hpp"

namespace Stand
{
	class CommandRc : public CommandToggleRegularSp
	{
	private:
		GhostDriver controller{};
		AbstractEntity last_veh{};
		time_t stopped_being_on_all_wheels = 0;
		int lights_state = 1;
		time_t last_turn = 0;

	public:
		explicit CommandRc(CommandList* const parent)
			: CommandToggleRegularSp(parent, LOC("RC"), { CMDNAME("rc"), CMDNAME("remotecontrol") })
		{
		}

	private:
		void cleanup()
		{
			if (controller.needsCleanup())
			{
				controller.cleanup();

				GhostDriver::user_is_rcing = false;

				g_overwriting_cam_ent = false;

				stopped_being_on_all_wheels = 0;

				NETWORK::SET_NETWORK_ID_CAN_MIGRATE(NETWORK::VEH_TO_NET(last_veh), TRUE);

				VEHICLE::SET_VEHICLE_LIGHTS(last_veh, 0);
				if (is_session_started())
				{
					HUD::SET_ABILITY_BAR_VISIBILITY(FALSE);
				}

				Camgr::unlock_focus();
				HUD::UNLOCK_MINIMAP_POSITION();
			}
		}

		static std::string getStuckMessage()
		{
			return LANG_FMT("RC_STCK", "~INPUT_VEH_CIN_CAM~");
		}

	public:
		void onEnable(Click& click) final
		{
			checkPermissions(click);
			registerScriptTickEventHandler(click, [this]
			{
				if (m_on)
				{
					AbstractEntity veh;
					if (!controller.initVehicle(veh))
					{
						cleanup();
					}
					else
					{
						AbstractEntity driver;
						controller.initDriver(driver);

						GhostDriver::user_is_rcing = true;

						g_overwriting_cam_ent = true;
						g_cam_veh = veh;
						AbstractEntity::updateCamEnt();

						last_veh = veh;
						auto pos = veh.getPos();

						NETWORK::NETWORK_REQUEST_CONTROL_OF_ENTITY(veh);
						NETWORK::SET_NETWORK_ID_CAN_MIGRATE(NETWORK::VEH_TO_NET(veh), FALSE);
						veh.startEngine();

						VEHICLE::SET_BIKE_ON_STAND(veh, 0.0f, 0.0f);
						if (VEHICLE::IS_VEHICLE_STUCK_TIMER_UP(veh, 0, 2500))
						{
							Util::show_corner_help(getStuckMessage(), false);
						}
						else if (
							ENTITY::IS_ENTITY_IN_WATER(veh)
								? (VEHICLE::IS_THIS_MODEL_AN_AMPHIBIOUS_CAR(veh.getModel()) || VEHICLE::IS_THIS_MODEL_AN_AMPHIBIOUS_QUADBIKE(veh.getModel()))
								: (VEHICLE::IS_VEHICLE_ON_ALL_WHEELS(veh) || ENTITY::GET_ENTITY_SPEED(veh) > 10.0f)
							)
						{
							stopped_being_on_all_wheels = 0;
						}
						else
						{
							if (stopped_being_on_all_wheels == 0)
							{
								stopped_being_on_all_wheels = get_current_time_millis();
							}
							else if (GET_MILLIS_SINCE(stopped_being_on_all_wheels) > 2500)
							{
								Util::show_corner_help(getStuckMessage(), false);
							}
						}
						if (Input::isControlJustPressed(INPUT_VEH_CIN_CAM))
						{
							Vector3 closestVehicleNodeCoords; 
							float roadHeading; 
							PATHFIND::GET_CLOSEST_VEHICLE_NODE_WITH_HEADING(pos.x, pos.y, pos.z, &closestVehicleNodeCoords, &roadHeading, 1, 3, 0);
							ENTITY::SET_ENTITY_HEADING(veh, roadHeading);
							ENTITY::SET_ENTITY_COORDS(veh, closestVehicleNodeCoords.x, closestVehicleNodeCoords.y, closestVehicleNodeCoords.z, 1, 0, 0, 1);

							if (Util::is_help_message_being_displayed(getStuckMessage()))
							{
								HUD::CLEAR_HELP(TRUE);
							}
						}
						if (PED::IS_PED_IN_MELEE_COMBAT(g_player_ped))
						{
							TASK::CLEAR_PED_TASKS_IMMEDIATELY(g_player_ped);
						}

						Camgr::lock_focus(pos);
						if (!g_player_veh.isValid())
						{
							CAM::SET_IN_VEHICLE_CAM_STATE_THIS_UPDATE(veh, 0);
							HUD::LOCK_MINIMAP_POSITION(pos.x, pos.y);
						}

						for (const auto& input : g_movement_inputs)
						{
							PAD::DISABLE_CONTROL_ACTION(0, input, TRUE);
						}

						if (!VEHICLE::GET_HAS_ROCKET_BOOST(veh))
						{
							if (Input::isControlJustPressed(INPUT_VEH_HORN))
							{
								VEHICLE::START_VEHICLE_HORN(veh, 1, ATSTRINGHASH("HELDDOWN"), TRUE);
							}
							else if (PAD::IS_DISABLED_CONTROL_JUST_RELEASED(0, INPUT_VEH_HORN))
							{
								VEHICLE::START_VEHICLE_HORN(veh, 1, ATSTRINGHASH("HELDDOWN"), FALSE);
							}
						}
						else
						{
							if (is_session_started())
							{
								HUD::SET_ABILITY_BAR_VISIBILITY(TRUE);
								HUD::SET_ABILITY_BAR_VALUE(veh.getCVehicle()->boost_charge, 1.25f);
							}
							if (Input::isControlJustPressed(INPUT_VEH_ROCKET_BOOST))
							{
								VEHICLE::SET_ROCKET_BOOST_ACTIVE(veh, TRUE);
							}
						}

						if (Input::isControlJustPressed(INPUT_VEH_HEADLIGHT))
						{
							if (++lights_state > 2)
							{
								lights_state = 0;
							}
							VEHICLE::SET_VEHICLE_USE_PLAYER_LIGHT_SETTINGS(veh, FALSE);
							if (lights_state == 0)
							{
								VEHICLE::SET_VEHICLE_LIGHTS(veh, 1);
							}
							else if (lights_state == 2)
							{
								VEHICLE::SET_VEHICLE_LIGHTS(veh, 2);
								VEHICLE::SET_VEHICLE_FULLBEAM(veh, TRUE);
							}
							else
							{
								VEHICLE::SET_VEHICLE_LIGHTS(veh, 2);
								VEHICLE::SET_VEHICLE_FULLBEAM(veh, FALSE);
							}
						}

						if (Input::isControlPressed(INPUT_VEH_HANDBRAKE))
						{
							if (Input::isControlPressed(INPUT_VEH_MOVE_LEFT_ONLY))
							{
								TASK::TASK_VEHICLE_TEMP_ACTION(driver, veh, 11, 60);
							}
							else if (Input::isControlPressed(INPUT_VEH_MOVE_RIGHT_ONLY))
							{
								TASK::TASK_VEHICLE_TEMP_ACTION(driver, veh, 10, 60);
							}
							else
							{
								TASK::TASK_VEHICLE_TEMP_ACTION(driver, veh, 6, 60);
							}
						}
						else if (Input::isControlPressed(INPUT_VEH_ACCELERATE))
						{
							if (Input::isControlPressed(INPUT_VEH_BRAKE))
							{
								TASK::TASK_VEHICLE_TEMP_ACTION(driver, veh, 30, 60);
							}
							else if (Input::isControlPressed(INPUT_VEH_MOVE_LEFT_ONLY))
							{
								last_turn = get_current_time_millis();
								TASK::TASK_VEHICLE_TEMP_ACTION(driver, veh, 7, 60);
							}
							else if (Input::isControlPressed(INPUT_VEH_MOVE_RIGHT_ONLY))
							{
								last_turn = get_current_time_millis();
								TASK::TASK_VEHICLE_TEMP_ACTION(driver, veh, 8, 60);
							}
							else
							{
								if (GET_MILLIS_SINCE(last_turn) > 1000)
								{
									TASK::TASK_VEHICLE_TEMP_ACTION(driver, veh, 23, 60);
								}
								else
								{
									TASK::TASK_VEHICLE_TEMP_ACTION(driver, veh, 9, 60);
								}
							}
						}
						else if (Input::isControlPressed(INPUT_VEH_BRAKE))
						{
							if (Input::isControlPressed(INPUT_VEH_MOVE_LEFT_ONLY))
							{
								TASK::TASK_VEHICLE_TEMP_ACTION(driver, veh, 13, 60);
							}
							else if (Input::isControlPressed(INPUT_VEH_MOVE_RIGHT_ONLY))
							{
								TASK::TASK_VEHICLE_TEMP_ACTION(driver, veh, 14, 60);
							}
							else
							{
								TASK::TASK_VEHICLE_TEMP_ACTION(driver, veh, 28, 60);
							}
						}
						else if (Input::isControlPressed(INPUT_VEH_MOVE_LEFT_ONLY))
						{
							TASK::TASK_VEHICLE_TEMP_ACTION(driver, veh, 11, 60);
						}
						else if (Input::isControlPressed(INPUT_VEH_MOVE_RIGHT_ONLY))
						{
							TASK::TASK_VEHICLE_TEMP_ACTION(driver, veh, 10, 60);
						}
						else
						{
							TASK::TASK_VEHICLE_TEMP_ACTION(driver, veh, 27, 60);
						}
					}
					return true;
				}
				cleanup();
				return false;
			});
		}
	};
}
