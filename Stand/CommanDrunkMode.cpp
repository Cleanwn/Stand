#pragma once

#include "CommandDrunkMode.hpp"

#include "AbstractEntity.hpp"
#include "CommandTimecycleMod.hpp"
#include "CommandWalkStyle.hpp"
#include "get_current_time_millis.hpp"
#include "Script.hpp"

namespace Stand
{
	CommandDrunkMode::CommandDrunkMode(CommandList* const parent)
		: CommandToggle(parent, LOC("DRNKMODE"), CMDNAMES_OBF("drunkmode"))
	{
	}

	void CommandDrunkMode::onEnable(Click& click)
	{
		CommandTimecycleMod::shader_overriden = true;
		CommandWalkStyle::walk_style_overridden = true;

		registerScriptTickEventHandler([this]
		{
			HANDLER_CHECK(m_on);

			if (!CAM::IS_GAMEPLAY_CAM_SHAKING())
			{
				CAM::SHAKE_GAMEPLAY_CAM("DRUNK_SHAKE", 2.0f);
			}

			AUDIO::SET_PED_IS_DRUNK(g_player_ped, TRUE);
			AUDIO::SET_AUDIO_SPECIAL_EFFECT_MODE(2);
			GRAPHICS::SET_TIMECYCLE_MODIFIER("Drunk");

			STREAMING::REQUEST_CLIP_SET("move_m@drunk@verydrunk");
			while (!STREAMING::HAS_CLIP_SET_LOADED("move_m@drunk@verydrunk"))
			{
				Script::current()->yield();
			}

			PED::SET_PED_MOVEMENT_CLIPSET(g_player_ped, "move_m@drunk@verydrunk", 1.0f);

			CAM::SET_GAMEPLAY_CAM_MOTION_BLUR_SCALING_THIS_UPDATE(1.0f);
			CAM::SET_GAMEPLAY_CAM_MAX_MOTION_BLUR_STRENGTH_THIS_UPDATE(1.0f);

			if (g_player_veh.isValid()
				&& g_player_veh.isOwnerOfVehicleAndDriver()
				&& ENTITY::GET_ENTITY_SPEED(g_player_veh) > 0
				)
			{
				if (soup::rand.one_in(10))
				{
					if (GET_MILLIS_SINCE(drunk_steer_delay) > 3000)
					{
						float rng = float(soup::rand(1, 5)) / 10.0f;
						steer_bias_goal = soup::rand.coinflip() ? -rng : rng;
						drunk_steer_delay = get_current_time_millis();
					}
					
					if (steer_bias < steer_bias_goal)
					{
						steer_bias += 0.1f;
					}
					else if (steer_bias > steer_bias_goal)
					{
						steer_bias -= 0.1f;
					}
				}
				else
				{
					steer_bias = 0.0f;
				}

				VEHICLE::SET_VEHICLE_STEER_BIAS(g_player_veh, steer_bias);
			}

			HANDLER_END;
		});
	}

	void CommandDrunkMode::onDisable(Click& click)
	{
		CommandTimecycleMod::shader_overriden = false;
		CommandWalkStyle::walk_style_overridden = false;

		click.ensureScriptThread([]
		{
			AUDIO::SET_PED_IS_DRUNK(g_player_ped, FALSE);
			PED::RESET_PED_MOVEMENT_CLIPSET(g_player_ped, 1.0f);
			CAM::STOP_GAMEPLAY_CAM_SHAKING(TRUE);
			STREAMING::REMOVE_CLIP_SET("move_m@drunk@verydrunk");
			CAM::SET_GAMEPLAY_CAM_MOTION_BLUR_SCALING_THIS_UPDATE(0.0f);
			CAM::SET_GAMEPLAY_CAM_MAX_MOTION_BLUR_STRENGTH_THIS_UPDATE(0.0f);
			GRAPHICS::CLEAR_TIMECYCLE_MODIFIER();
		});
	}
}