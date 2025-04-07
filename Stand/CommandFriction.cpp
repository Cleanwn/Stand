#include "CommandFriction.hpp"

#include "AbstractEntity.hpp"
#include "CPlayerInfo.hpp"
#include "gta_input.hpp"
#include "input.hpp"
#include "natives.hpp"
#include "TickMgr.hpp"

namespace Stand
{
	CommandFriction::CommandFriction(CommandList* const parent)
		: CommandToggle(parent, LOC("INCFRCT"), CMDNAMES("friction"), LOC("INCFRCT_H"))
	{
	}

	void CommandFriction::onEnable(Click& click)
	{
		registerScriptTickEventHandler(click, [this]
		{
			HANDLER_CHECK(m_on)
			if (g_player_ped.isOnGround())
			{
				auto vel = ENTITY::GET_ENTITY_VELOCITY(g_player_ped);
				vel.normalise();
				const auto vel_z = vel.z;
				vel.z = 0.0f;

				v3 dir{};
				if (Input::canMovementCommandPerformMovement())
				{
					auto cam_rot = CAM::GET_FINAL_RENDERED_CAM_ROT(2);
					if (Input::isControlPressed(INPUT_LOOK_BEHIND)
						&& !Input::isControlPressed(INPUT_SELECT_WEAPON)
						)
					{
						cam_rot.z += 180.0f;
					}
					if (Input::isControlPressed(INPUT_MOVE_UP_ONLY))
					{
						dir += cam_rot.toDirNoZ();
					}
					if (Input::isControlPressed(INPUT_MOVE_DOWN_ONLY))
					{
						dir -= cam_rot.toDirNoZ();
					}
					if (Input::isControlPressed(INPUT_MOVE_LEFT_ONLY))
					{
						dir += v3(0, 0, cam_rot.z + 90.0f).toDirNoZ();
					}
					if (Input::isControlPressed(INPUT_MOVE_RIGHT_ONLY))
					{
						dir += v3(0, 0, cam_rot.z - 90.0f).toDirNoZ();
					}
					if (!dir.isNull())
					{
						dir.normalise();
						dir.z = 0.0f;
					}
				}

				float speed = g_player_ped.getPlayerInfo()->m_run_speed;
				if (Input::isControlPressed(INPUT_SPRINT))
				{
					speed *= 5.0f;
				}

				if (!TickMgr::is_super_jumping)
				{
					if (!dir.isNull())
					{
						if (vel.dot(dir) < 0.95f)
						{
							ENTITY::SET_ENTITY_VELOCITY(g_player_ped, dir.x * speed, dir.y * speed, vel_z * speed);
							reset_vel = false;
						}
					}
					else if (!reset_vel)
					{
						reset_vel = true;
						ENTITY::SET_ENTITY_VELOCITY(g_player_ped, 0.0f, 0.0f, 0.0f);
					}
				}
			}
			HANDLER_END;
		});
	}
}
