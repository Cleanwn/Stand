#include "CommandAutoDriveAction.hpp"

#include "AbstractModel.hpp"
#include "BlipUtil.hpp"
#include "Box.hpp"
#include "CommandGhostVehicles.hpp"
#include "CommandSeatglue.hpp"
#include "DrawUtil3d.hpp"
#include "eDrivingFlags.hpp"
#include "eScriptTaskStatus.hpp"
#include "FiberPool.hpp"
#include "Gui.hpp"
#include "input.hpp"
#include "PlaceObjective.hpp"
#include "Script.hpp"
#include "Util.hpp"

namespace Stand
{
	Vector2Plus CommandAutoDriveAction::getDestinationCoord(Click& click, uint16_t destination)
	{
		switch (destination)
		{
		case NOWHERE:
			break;

		case WAYPOINT:
			if (auto waypoint = g_player.getWaypoint(); waypoint.has_value())
			{
				return waypoint.value();
			}
			break;

		case OBJECTIVE:
			{
				PlaceObjective objective;
				auto pop = objective.getClosestPop(click);
				if (pop.has_value())
				{
					return pop.value().pos;
				}
			}
			break;

		default:
			{
				Vector2Plus blip_coord = BlipUtil::closestBlipCoordOfType(destination);
				blip_coord.unguessZ(0.0f);

				if (isSingular(destination))
				{
					for (const auto& predefined : predefined_areas)
					{
						if (predefined.id == destination && predefined.area.distanceTopdown(blip_coord) < 40.0f)
						{
							return predefined.area;
						}
					}

					return blip_coord;
				}
				else
				{
					std::optional<Vector2Plus> closest_predefined_location;

					for (const auto& predefined : predefined_areas)
					{
						if (predefined.id == destination)
						{
							if (!closest_predefined_location.has_value())
							{
								closest_predefined_location = predefined.area;
							}
							else
							{
								const auto pos = g_player_ped.getPos();
								if (closest_predefined_location.value().distanceTopdown(pos) > predefined.area.distanceTopdown(pos))
								{
									closest_predefined_location = predefined.area;
								}
							}
						}
					}

					return closest_predefined_location.value_or(destination == STORE ? Vector2Plus() : blip_coord); // Remove special store logic once the proper blip sprite id is found.
				}
			}
			break;
		}

		return {};
	}

	float CommandAutoDriveAction::getCruiseSpeed() const
	{
		if (const auto overridden_speed = speed->getFloatValue(); overridden_speed != 0.0f)
		{
			return overridden_speed;
		}

		if ((eDrivingFlags)mode->value != eDrivingFlags::CUSTOM_Aggressive)
		{
			std::vector<float> speeds;

			AbstractEntity::getAllVehicles([&speeds, vehicle { this->vehicle }](AbstractEntity&& ent) mutable
			{
				const auto speed = ENTITY::GET_ENTITY_SPEED(ent);
				const auto our_pos = vehicle.getPos();
				const auto their_pos = ent.getPos();

				if (speed > default_cruise_speed // Don't want 'clogged up' NPCs to influence our cruise speed.
					&& std::abs(our_pos.z - their_pos.z) <= 3.0f
					&& !ent.getDriver().isAPlayer()
					)
				{
					speeds.emplace_back(speed);
	#ifdef STAND_DEBUG
					ent.getBox().drawLines(255, 255, 255, 255);
	#endif
				}

				CONSUMER_CONTINUE;	
			});

			if (!speeds.empty())
			{
				float average = 0.0f;

				for (const auto& v : speeds)
				{
					average += v;
				}

				return average / speeds.size();
			}

			return default_cruise_speed;
		}

		return 80.0f;
	}

	bool CommandAutoDriveAction::isPathPredefined(const v3& path)
	{
		for (const auto& predefined : predefined_areas)
		{
			if (predefined.area == path)
			{
				return true;
			}
		}

		return false;
	}

	bool CommandAutoDriveAction::isSingular(uint16_t destination) const
	{
		switch (destination)
		{
		case AGENCY:
		case OFFICE:
		case HANGAR:
		case FACILITY:
		case CLUBHOUSE:
			return true;
		}

		return false;
	}

	bool CommandAutoDriveAction::isAttemptingToMove() const
	{
		return Input::canMovementCommandPerformMovement()
			&& (Input::isControlPressed(INPUT_VEH_MOVE_LEFT_ONLY)
				|| Input::isControlPressed(INPUT_VEH_MOVE_RIGHT_ONLY)
				|| Input::isControlPressed(INPUT_VEH_ACCELERATE)
				|| Input::isControlPressed(INPUT_VEH_HANDBRAKE)
				|| Input::isControlPressed(INPUT_VEH_BRAKE)
			);
	}

	CommandAutoDriveAction::CommandAutoDriveAction(CommandList* const parent)
		: CommandListAction(parent, LOC("AUTODRIVE_DEST"), CMDNAMES_OBF("goto"), NOLABEL, {
				{ NOWHERE, LOC("AUTODRIVE_DEST_NONE"), CMDNAMES_OBF("none"), LOC("AUTODRIVE_DEST_NONE_H") },
				{ WAYPOINT, LOC("WP"), CMDNAMES_OBF("waypoint") },
				{ OBJECTIVE, LOC("OBJCTV"), CMDNAMES_OBF("objective") },
				{ CASINO, LOC("CSNO"), CMDNAMES_OBF("casino") },
				{ AMMUNATION, LOC("AMMUNAT"), CMDNAMES_OBF("ammunation") },
				{ CLOTHES, LOC("CLTHST"), CMDNAMES_OBF("clothesstore") },
				{ TATS, LOC("TATTOO"), CMDNAMES_OBF("tattooparlor") },
				{ SALON, LOC("BARBR"), CMDNAMES_OBF("barber") },
				{ LSC, LOC("LSC"), CMDNAMES_OBF("lossantoscustoms", "lsc") },
				{ MOVIES, LOC("AUTODRIVE_MOVIE"), CMDNAMES_OBF("movies") },
				{ STORE, LOC("AUTODRIVE_STORE"), CMDNAMES_OBF("store") },
				{ CLUBHOUSE, LOC("AUTODRIVE_CLUBHOUSE"), CMDNAMES_OBF("mc", "clubhouse") },
				{ FACILITY, LOC("AUTODRIVE_FACILITY"), CMDNAMES_OBF("facility") },
				{ AGENCY, LOC("AUTODRIVE_AGENCY"), CMDNAMES_OBF("agency") },
				{ OFFICE, LOC("AUTODRIVE_OFFICE"), CMDNAMES_OBF("office") },
				{ HANGAR, LOC("AUTODRIVE_HANGAR"), CMDNAMES_OBF("hangar") },
			})
	{
	}

	void CommandAutoDriveAction::onItemClick(Click& click, CommandListActionItem* item)
	{
		ensureScriptThread([=]() mutable
		{
			if (!g_player_veh.isValid() || !g_player_veh.isOwnerOfVehicleAndDriver())
			{
				click.setResponse(LOC("NOVEHFND"));
				return;
			}

			auto model = g_player_veh.getModel();
			if (model.isAquatic() || model.isAirborne())
			{
				click.setResponse(LOC("AUTODRIVE_ERR2"));
				return;
			}

			auto destination = getDestinationCoord(click, (uint16_t)item->value);
			if (item->value != NOWHERE && destination.isNull())
			{
				click.setResponse(LOC("AUTODRIVE_ERR3"));
			}
			else if (item->value != NOWHERE && destination.distanceTopdown(g_player_ped.getPos()) < 40.0f)
			{
				click.setResponse(LOC("AUTODRIVE_ERR4"));
			}
			else
			{
				vehicle = g_player_veh;
				applyBehaviour(std::move(destination));
				doTickHandler();
			}
		});
	}

	void CommandAutoDriveAction::applyBehaviour(Vector2Plus&& destination)
	{
		const auto flags = (int)mode->value;
		TASK::CLEAR_PED_TASKS(g_player_ped);

		if (destination.isNull())
		{
			destination_coords.reset();
			TASK::TASK_VEHICLE_DRIVE_WANDER(g_player_ped, vehicle, default_cruise_speed, flags);
		}
		else
		{
			if (!vehicle.isOnRoad())
			{
#ifdef STAND_DEBUG
				Util::toast("We are not on a road, temporarily enabling wander mode.");
#endif
				TASK::TASK_VEHICLE_DRIVE_WANDER(g_player_ped, vehicle, default_cruise_speed, flags); // Much better offroad pathfinding. Can get itself off a mountain.
			}
			else
			{
				TASK::TASK_VEHICLE_DRIVE_TO_COORD_LONGRANGE(g_player_ped, vehicle, destination.x, destination.y, destination.z, default_cruise_speed, flags, 0.0f);
			}
			destination_coords = destination;
		}
	}

	void CommandAutoDriveAction::doTickHandler()
	{
		if (!handler_running)
		{
			handler_running = true;
			registerScriptTickEventHandler([this]
			{
				if (Input::isControlJustPressed(INPUT_VEH_EXIT))
				{
					kill();
					TASK::TASK_LEAVE_VEHICLE(g_player_ped, vehicle, 0);
					return false;
				}
				else if (should_die
					|| g_gui.isUnloadPending()
					|| vehicle != g_player_veh
					|| !vehicle.isOwnerOfVehicleAndDriver()
					|| (auto_kill->m_on && isAttemptingToMove())
					)
				{
					kill();
					return false;
				}
				else if (!destination_coords.isNull())
				{
					if (beacon->m_on)
					{
						DrawUtil3d::draw_ar_beacon(destination_coords);
					}

					if (TASK::GET_SCRIPT_TASK_STATUS(g_player_ped, ATSTRINGHASH("script_task_vehicle_drive_wander")) == PERFORMING_TASK && vehicle.isOnRoad()) // The only time a destination is set with wander mode, is when we are looking for a road.
					{
#ifdef STAND_DEBUG
						Util::toast("We found a road, killing wander task.");
#endif
						TASK::CLEAR_PED_TASKS(g_player_ped);
						TASK::TASK_VEHICLE_DRIVE_TO_COORD_LONGRANGE(g_player_ped, vehicle, destination_coords.x, destination_coords.y, destination_coords.z, default_cruise_speed, flags, 0.0f);
					}

					const auto distance = vehicle.getPos().distanceTopdown(destination_coords);
					if (distance < (isPathPredefined(destination_coords) ? 10.0f : 40.0f))
					{
						Util::toast(LOC("AUTODRIVE_ARRIVE"));

						FiberPool::queueJob([]
						{
							VEHICLE::BRING_VEHICLE_TO_HALT(g_player_veh, 10.0f, 2000, FALSE);
							Script::current()->yield(2000);
							VEHICLE::STOP_BRINGING_VEHICLE_TO_HALT(g_player_veh);
						});

						kill();
						return false;
					}
					else if (distance < 70.0f)
					{
						TASK::SET_DRIVE_TASK_CRUISE_SPEED(g_player_ped, default_cruise_speed);
						HANDLER_END;
					}
				}

#ifdef STAND_DEBUG
				if (getCruiseSpeed() && GET_MILLIS_SINCE(last_speed_update) > 1000) // We do this to ensure we draw the box every tick. Setting speed too often causes the task update to fail.
#else
				if (GET_MILLIS_SINCE(last_speed_update) > 1000)
#endif
				{
					const auto speed = getCruiseSpeed();
					TASK::SET_DRIVE_TASK_CRUISE_SPEED(g_player_ped, speed);
					TASK::SET_DRIVE_TASK_MAX_CRUISE_SPEED(g_player_ped, speed);
					last_speed_update = get_current_time_millis();
				}

				if (noclipping->m_on)
				{
					g_gui.no_collision_with_other_vehicles = true;
				}

				if (const auto model = vehicle.getModel(); model.isBike() || model.isBicycle())
				{
					g_gui.seatglue = true;
				}

				HANDLER_END;
			});
		}
	}

	void CommandAutoDriveAction::requestDeath()
	{
		if (handler_running)
		{
			should_die = true;
		}
	}

	void CommandAutoDriveAction::kill()
	{
		TASK::CLEAR_PED_TASKS(g_player_ped);
		destination_coords.reset();
		last_speed_update = 0;
		handler_running = false;
		should_die = false;
		vehicle.reset();

		g_gui.no_collision_with_other_vehicles = CommandGhostVehicles::instance->m_on;
		g_gui.seatglue = CommandSeatglue::instance->m_on;
	}
}