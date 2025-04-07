#include "CommandBombTrajectory.hpp"

#include "AbstractEntity.hpp"
#include "DrawUtil3d.hpp"
#include "get_ground_z.hpp"
#include "natives.hpp"

namespace Stand
{
	CommandBombTrajectory::CommandBombTrajectory(CommandList* const parent)
		: CommandToggle(parent, LOC("BMBTRJC"), CMDNAMES("bombtrajectory", "showbombtrajectory"))
	{
	}

	void CommandBombTrajectory::onChange(Click& click)
	{
		onChangeToggleScriptTickEventHandler(click, []
		{
			if (VEHICLE::GET_ARE_BOMB_BAY_DOORS_OPEN(g_player_veh))
			{
				auto bomb_pos = g_player_veh.getBombBayPos().first;

				constexpr float gravity = 9.81f;
				constexpr float time_step = 0.1f;
				constexpr float air_resistance = 0.99f; // print(2.718282 ^ (-0.1 * time_step))

				auto vel = ENTITY::GET_ENTITY_SPEED_VECTOR(g_player_veh, FALSE); // TODO: Might be interesting to consider CAmmoInfo::m_LaunchSpeed
				vel.z *= gravity * time_step;

				for (int i = 0; i != 1000; ++i)
				{
					const auto ground_z = get_ground_z(bomb_pos.x, bomb_pos.y);
					if (bomb_pos.z <= ground_z)
					{
						break;
					}
					auto prev_bomb_pos = bomb_pos;
					bomb_pos += vel * time_step;
					bomb_pos.z -= gravity * time_step;
					vel.z -= gravity * time_step;

					vel.x *= air_resistance;
					vel.y *= air_resistance;

					DrawUtil3d::draw_line(prev_bomb_pos, bomb_pos);
				}
			}
		});
	}
}
