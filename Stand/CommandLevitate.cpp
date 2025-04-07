#include "CommandLevitate.hpp"

#include "AbstractEntity.hpp"
#include "ButtonInstructions.hpp"
#include "CommandSlider.hpp"
#include "CPedIntelligence.hpp"
#include "eTaskType.hpp"
#include "FiberPool.hpp"
#include "free_movement.hpp"
#include "get_ground_z.hpp"
#include "gta_input.hpp"
#include "gta_ped.hpp"
#include "gta_task.hpp"
#include "gta_vehicle.hpp"
#include "Gui.hpp"
#include "input.hpp"
#include "is_session.hpp"
#include "natives.hpp"
#include "paused.hpp"
#include "Script.hpp"
#include "tbFreecam.hpp"

namespace Stand
{
	void CommandLevitate::onChange(Click& click)
	{
		levitate_is_on = m_on;
		CommandToggle::onChange(click);
	}

	bool CommandLevitate::isPedTooHigh() noexcept
	{
		if (g_player_ent.isPed())
		{
			return ENTITY::GET_ENTITY_HEIGHT_ABOVE_GROUND(g_player_ent) > 1.0f;
		}

		return false;
	}

	bool CommandLevitate::isFirstPerson() noexcept
	{
		return CAM::GET_CAM_VIEW_MODE_FOR_CONTEXT(CAM::GET_CAM_ACTIVE_VIEW_MODE_CONTEXT()) == 4;
	}

	void CommandLevitate::onEnable(Click& click)
	{
		ensureScriptThread(click, [this]
		{
			z_extra_alt = false;
			z_extra = 0.0f;
			registerScriptTickEventHandler(TC_SCRIPT_NOYIELD, [this]
			{
				if (!m_on)
				{
					ENTITY::FREEZE_ENTITY_POSITION(g_player_ped, FALSE);

					if (!is_session_started()
						&& isFirstPerson()
						)
					{
						ENTITY::SET_ENTITY_VISIBLE(g_player_ped, TRUE, FALSE);
					}

					if (keep_momentum->m_on
						&& momentum_time != 0
						&& !momentum_pos.isNull()
						)
					{
						auto diff = get_current_time_millis() - momentum_time;
						auto delta = (diff / 1000.0f) * 1.5f;
						auto delta_pos = (g_player_ent.getPos() - momentum_pos) / delta;

						ENTITY::SET_ENTITY_VELOCITY(g_player_ent, delta_pos.x, delta_pos.y, delta_pos.z);

						if (g_player_ent.isVehicle())
						{
							SOUP_IF_LIKELY (auto cveh = g_player_veh.getCVehicle())
							{
								cveh->m_Transmission.m_nGear = 1; // The game locks us into reverse for some reason.
							}
						}

						// If we're above the ground, enable grace until we're settled.
						// Some natural motion tasks (even when killed) result in our velocity being reset.
						if (isPedTooHigh() && !grace_job_busy)
						{
							++g_gui.grace;
							grace_job_busy = true;
							FiberPool::queueJob([this]
							{
								while (keep_momentum->m_on && isPedTooHigh())
								{
									Script::current()->yield(10);
								}

								grace_job_busy = false;
								--g_gui.grace;
							});
						}
					}

					last_set_pos.reset();
					momentum_pos.reset();
					momentum_time = 0;
					speed = 0.0f;
					return false;
				}

				if (speed == 0.0f)
				{
					speed = base_speed->getFloatValue();
				}
				
				if (keep_momentum->m_on)
				{
					momentum_pos = g_player_ent.getPos();
					momentum_time = get_current_time_millis();
				}
				else
				{
					momentum_pos.reset();
					momentum_time = 0;
				}

				if (!g_paused)
				{
					const bool can_move = Input::canMovementCommandPerformMovement();

					if (can_move && show_button_instructions->m_on)
					{
						ButtonInstructions::setFreeMovementThisTick();
					}

					v3 pos = g_player_ent.getPos();
					if (pos.distance(last_set_pos) < 5.0f)
					{
						pos = last_set_pos;
					}
					pos.z -= z_extra;
					float perfect_z = pos.z;
					v3 rot;
					if (g_tb_freecam.isEnabled())
					{
						rot = g_player_ent.getRot();
					}
					else
					{
						rot = CAM::GET_FINAL_RENDERED_CAM_ROT(2);
					}
					rot.y = 0.0f;
					if (PAD::IS_DISABLED_CONTROL_PRESSED(0, INPUT_LOOK_BEHIND))
					{
						rot.z += 180.0f;
					}
					bool moved = false;
					if (can_move)
					{
						const auto pitch = rot.x;
						if (ignore_pitch->m_on)
						{
							rot.x = 0.0f;
						}
						moved = free_movement(pos, rot, speed, sprint_speed->getFloatValue());

						if (acceleration->value != 0 && moved && !Input::isControlPressed(Input::getFreeSprint()))
						{
							if (const auto new_v = speed + (acceleration->getFloatValue() / 100); new_v < 12.5f) // Stuttering begins to occur around this range. (for high-end systems, at least)
							{
								speed = new_v;
							}
						}
						else
						{
							speed = base_speed->getFloatValue();
						}

						rot.x = pitch;
					}
					if (!apply_pitch_to_entity->m_on)
					{
						rot.x = 0.0f;
					}
					// Assistant: Calculate "perfect Z"
					{
						bool using_z_from_heightmap = false;
					shoot_ray:
						float ground_z_shapetest = get_ground_z_shapetest(pos.x, pos.y, perfect_z);
						float ground_z_water = get_ground_z_water(pos.x, pos.y);

						static constexpr float mode_switch_threshold = 1.5f;

						if (water)
						{
							if (abs(ground_z_shapetest - pos.z) < mode_switch_threshold && abs(ground_z_water - pos.z) > mode_switch_threshold)
							{
								water = false;
							}
						}
						else
						{
							if (abs(ground_z_water - pos.z) < mode_switch_threshold && abs(ground_z_shapetest - pos.z) > mode_switch_threshold)
							{
								water = true;
							}
						}

						if (water)
						{
							if (ground_z_water != -200.0f)
							{
								perfect_z = ground_z_water;
							}
							else
							{
								if (ground_z_shapetest != -200.0f)
								{
									perfect_z = ground_z_shapetest;
									water = false;
								}
								else
								{
									if (!using_z_from_heightmap)
									{
										perfect_z = pos.getZFromHeightmap();
										using_z_from_heightmap = true;
										goto shoot_ray;
									}
								}
							}
						}
						else
						{
							if (ground_z_shapetest != -200.0f)
							{
								if (ground_z_water != -200.0f && ground_z_water > ground_z_shapetest)
								{
									perfect_z = ground_z_water;
									water = true;
								}
								else
								{
									perfect_z = ground_z_shapetest;
								}
							}
							else
							{
								if (using_z_from_heightmap)
								{
									if (ground_z_water != -200.0f)
									{
										perfect_z = ground_z_water;
										water = true;
									}
								}
								else
								{
									perfect_z = pos.getZFromHeightmap();
									using_z_from_heightmap = true;
									goto shoot_ray;
								}
							}
						}
					}
					perfect_z += g_player_ent.getDimensions().z;
					// Assistant: Nudge towards "perfect Z"
					if (abs(pos.z - perfect_z) < (float)assist_snap->value / 100.0f)
					{
						pos.z = perfect_z;
					}
					else if (pos.z - perfect_z < (float)assist_deadzone->value)
					{
						if (pos.z < perfect_z)
						{
							pos.z += (float)assist_up->value / 100.0f;
							if (pos.z > perfect_z)
							{
								pos.z = perfect_z;
							}
						}
						else if (pos.z > perfect_z)
						{
							pos.z -= (float)assist_down->value / 100.0f;
							if (pos.z < perfect_z)
							{
								pos.z = perfect_z;
							}
						}
					}
					// Up & down hovering effect
					if (passive_min->value == passive_max->value)
					{
						z_extra = (float)passive_min->value / 100.0f;
					}
					else
					{
						auto passive_min = (float)this->passive_min->value / 100.0f;
						auto passive_max = (float)this->passive_max->value / 100.0f;
						if (passive_max < passive_min)
						{
							std::swap(passive_min, passive_max);
						}
						if (z_extra_alt)
						{
							z_extra -= (float)passive_speed->value / 1000.0f;
							if (z_extra <= passive_min)
							{
								z_extra_alt = false;
							}
						}
						else
						{
							z_extra += (float)passive_speed->value / 1000.0f;
							if (z_extra >= passive_max)
							{
								z_extra_alt = true;
							}
						}
					}
					pos.z += z_extra;

					// Commit
					ENTITY::FREEZE_ENTITY_POSITION(g_player_ped, TRUE);

					if (!is_session_started())
					{
						ENTITY::SET_ENTITY_VISIBLE(g_player_ped, !isFirstPerson(), FALSE);
					}

					bool set_rot = !only_rotate_on_movement->m_on;
					if (PED::IS_PED_IN_PARACHUTE_FREE_FALL(g_player_ped)
						|| TASK::GET_IS_TASK_ACTIVE(g_player_ped, CTaskTypes::TASK_PARACHUTE)
						)
					{
						g_player_ent.setPos(pos);
						last_set_pos = pos;
						set_rot = true;
					}
					else if (!g_player_ent.isPed())
					{
						tbFreecam::ensureNotInVehicleFirstPerson(0);
						g_player_ent.setPos(pos);
						last_set_pos = pos;
						if (moved)
						{
							set_rot = true;
						}
					}
					else if (isFirstPerson())
					{
						if (g_player_ent.getPos() != pos)
						{
							g_player_ent.setPos(pos);
							last_set_pos = pos;
							set_rot = true;
						}
					}
					else if (g_player_ent.getPos() != pos)
					{
						g_player_ent.setPos(pos);
						last_set_pos = pos;
						if (moved)
						{
							set_rot = true;
						}
					}
					
					if (set_rot)
					{
						g_player_ent.setRot(rot);
					}

					// Prevent rolling and other stupid shit
					for (const int input : g_movement_inputs)
					{
						PAD::DISABLE_CONTROL_ACTION(0, input, true);
					}

					// We want to retain control over the ped
					if (g_player_ent.isPed())
					{
						if (auto cped = g_player_ent.getCPed())
						{
							if (auto task = cped->intelligence->FindTaskActiveByType(CTaskTypes::TASK_FALL))
							{
								task->MakeAbortable(rage::aiTask::ABORT_PRIORITY_URGENT);
							}
						}
					}
				}
				return true;
			});
		});
	}
}
