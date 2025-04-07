#include "CommandSuperFlight.hpp"

#include "AbstractEntity.hpp"
#include "ButtonInstructions.hpp"
#include "eTaskType.hpp"
#include "get_ground_z.hpp"
#include "gta_input.hpp"
#include "Gui.hpp"
#include "Hooking.hpp"
#include "input.hpp"
#include "natives.hpp"
#include "tbTempEmptyParachute.hpp"
#include "TickMgr.hpp"

namespace Stand
{
	CommandSuperFlight::CommandSuperFlight(CommandList* const parent)
		: CommandSliderFloat(parent, LOC("FLY"), { CMDNAME("superflight"), CMDNAME("flight"), CMDNAME("fly") }, NOLABEL, -1000000, 1000000, 0, 25)
	{
	}

	std::wstring CommandSuperFlight::formatNumber(int num, bool allow_replacements) const
	{
		if (num == 0 && allow_replacements)
		{
			return LANG_GET_W("DOFF");
		}

		return CommandSliderFloat::formatNumber(num, allow_replacements);
	}

	void CommandSuperFlight::onChange(Click& click, int prev_value)
	{
		if (value != 0)
		{
			int value = this->value;
			registerScriptTickEventHandler(click, [this, value]()
			{
				if (this->value != value)
				{
					return false;
				}
				if (!g_player_ped.isInAnyVehicle())
				{
					if (mode == DEFAULT)
					{
						if (TASK::GET_IS_TASK_ACTIVE(g_player_ped, CTaskTypes::TASK_PARACHUTE))
						{
							mode = FLYING;
							onTickFlying();
						}
						else if (TickMgr::super_jump.isActiveThisTick())
						{
							if (Input::isContextAvailable())
							{
								if (TickMgr::is_super_jumping_for_user
									? (!TASK::GET_IS_TASK_ACTIVE(g_player_ped, CTaskTypes::TASK_FALL)
										|| getDistanceToGround() > 5.0f
										)
									: (TASK::GET_IS_TASK_ACTIVE(g_player_ped, CTaskTypes::TASK_FALL)
										&& getDistanceToGround() > 5.0f
										)
									)
								{
									if (bi_jump2fly)
									{
										ButtonInstructions::feature_buttons |= ButtonInstructions::SUPER_FLIGHT;
									}

									if (Input::isContextJustPressed())
									{
										mode = SUPER_JUMP_TO_FLIGHT;
										onTickStartup();
									}
								}
							}
						}
						else
						{
							onTickStartup();
						}
					}
					else if (mode == SUPER_JUMP_TO_FLIGHT)
					{
						onTickStartup();
					}
					else if (mode == FLYING)
					{
						onTickFlying();
					}
					else if (mode == DISABLING)
					{
						if (g_player_ped.isOnGround()
							&& !TASK::GET_IS_TASK_ACTIVE(g_player_ped, CTaskTypes::TASK_FALL)
							)
						{
							reset();
						}
						else
						{
							ENTITY::APPLY_FORCE_TO_ENTITY_CENTER_OF_MASS(g_player_ped, 1, 0.0f, 0.0f, -1.0f, true, false, true, true);
						}
					}
				}
				else
				{
					reset();
				}
				return true;
			});
		}
		else
		{
			reset();
		}
	}

	float CommandSuperFlight::getDistanceToGround()
	{
		auto pos = g_player_ped.getPos();
		return pos.z - get_ground_z(pos.x, pos.y, pos.z);
	}

	void CommandSuperFlight::onTickStartup()
	{
		if (TASK::GET_IS_TASK_ACTIVE(g_player_ped, CTaskTypes::TASK_PARACHUTE))
		{
			mode = FLYING;
			onTickFlying();
		}
		else if (TASK::GET_IS_TASK_ACTIVE(g_player_ped, CTaskTypes::TASK_FALL))
		{
			TASK::TASK_SKY_DIVE(g_player_ped, FALSE);
		}
		else
		{
			if (!TickMgr::is_super_jumping)
			{
				PAD::DISABLE_CONTROL_ACTION(0, INPUT_JUMP, TRUE);
				if (Input::isControlJustPressed(INPUT_JUMP))
				{
					TickMgr::doSuperJump();
				}
			}
			else if (getDistanceToGround() < 10.0f)
			{
				ENTITY::APPLY_FORCE_TO_ENTITY_CENTER_OF_MASS(g_player_ped, 1, 0.0f, 0.0f, 1.0f, true, false, true, true);
			}
		}
	}

	void CommandSuperFlight::onTickFlying()
	{
		PAD::DISABLE_CONTROL_ACTION(0, INPUT_PARACHUTE_DEPLOY, TRUE);
		PAD::DISABLE_CONTROL_ACTION(0, INPUT_PARACHUTE_PITCH_UD, TRUE);

		if (const auto ground_dist = getDistanceToGround();
			ground_dist < 1.0f
			|| (Input::isMenuClosedOrControllerUnused() && Input::isControlJustPressed(Input::keyboard_and_mouse ? INPUT_PARACHUTE_DEPLOY : INPUT_FRONTEND_Y))
			)
		{
			if (ground_dist < 5.0f && g_player_ped.hasHeadroom())
			{
				auto pos = g_player_ped.getPos();
				pos.z += (5.0f - ground_dist);
				g_player_ped.setPos(pos);
			}
			mode = DISABLING;
			if (!enabled_graceful_landing)
			{
				enabled_graceful_landing = true;
				++g_hooking.graceful_landing;
				g_tb_temp_empty_parachute.enable();
			}
			TASK::CLEAR_PED_TASKS_IMMEDIATELY(g_player_ped);
			ENTITY::SET_ENTITY_VELOCITY(g_player_ped, 0.0f, 0.0f, 0.0f);
			return;
		}

		if (!enabled_gracefulness)
		{
			enabled_gracefulness = true;
			++g_gui.grace;
		}

		if (TASK::GET_IS_TASK_ACTIVE(g_player_ped, CTaskTypes::TASK_FALL_OVER))
		{
			TASK::TASK_SKY_DIVE(g_player_ped, FALSE);
		}
		else if (!TASK::GET_IS_TASK_ACTIVE(g_player_ped, CTaskTypes::TASK_PARACHUTE))
		{
			reset();
			return;
		}

		const bool can_move = Input::canMovementCommandPerformMovement();

		if (bi_controls && Input::isMenuClosedOrControllerUnused())
		{
			ButtonInstructions::feature_buttons |= ButtonInstructions::LAND;
			if (can_move)
			{
				ButtonInstructions::feature_buttons |= ButtonInstructions::ASC_DESC;
			}
		}

		v3 dir{};
		if (can_move)
		{
			const auto speed = getFloatValue();
			dir -= (g_player_ped.getRot().toDirNoZ() * Input::getControlNormal(INPUT_MOVE_UD));
			dir.x *= speed;
			dir.y *= speed;
			if (Input::isAscDescAvailable())
			{
#if !CONTROLLER_ANALOG_ASCDESC
				if (Input::isController())
				{
					if (Input::isControlPressed(Input::getAscend()))
					{
						dir.z += speed;
					}
					else if (Input::isControlPressed(Input::getDescend()))
					{
						dir.z -= speed;
					}
					else
					{
						goto _no_asc_desc;
					}
				}
				else
#endif
				{
					auto norm = Input::getControlNormal(Input::getAscend(), Input::getDescend());
					if (norm == 0.0f)
					{
						goto _no_asc_desc;
					}
					dir.z += (speed * norm);
				}
			}
			else
			{
			_no_asc_desc:
				dir.z -= ENTITY::GET_ENTITY_VELOCITY(g_player_ped).z;
			}
		}
		else
		{
			PAD::DISABLE_CONTROL_ACTION(0, INPUT_PARACHUTE_TURN_LR, TRUE);
			dir.z -= ENTITY::GET_ENTITY_VELOCITY(g_player_ped).z;
		}
		ENTITY::APPLY_FORCE_TO_ENTITY_CENTER_OF_MASS(g_player_ped, 1, dir.x, dir.y, dir.z, true, false, true, true);
	}

	void CommandSuperFlight::reset()
	{
		mode = DEFAULT;
		if (enabled_graceful_landing)
		{
			enabled_graceful_landing = false;
			--g_hooking.graceful_landing;
			g_tb_temp_empty_parachute.disable();
		}
		if (enabled_gracefulness)
		{
			enabled_gracefulness = false;
			--g_gui.grace;
		}
	}
}
