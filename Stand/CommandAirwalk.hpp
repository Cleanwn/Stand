#pragma once

#include "CommandToggle.hpp"

#include "AbstractEntity.hpp"
#include "CPlayerInfo.hpp"
#include "natives.hpp"
#include "eTaskType.hpp"

namespace Stand
{
	class CommandAirwalk : public CommandToggle
	{
	public:
		explicit CommandAirwalk(CommandList* const parent)
			: CommandToggle(parent, LOC("WALKONAIR"), { CMDNAME("walkonair"), CMDNAME("airwalk") })
		{
		}

		void onChange(Click& click) final
		{
			registerScriptTickEventHandler(click, [=]()
			{
				if (!m_on)
				{
					ENTITY::SET_ENTITY_HAS_GRAVITY(g_player_ped, true);
					PED::SET_PED_GRAVITY(g_player_ped, true);
					return false;
				}
				if (PED::IS_PED_RAGDOLL(g_player_ped))
				{
					ENTITY::SET_ENTITY_HAS_GRAVITY(g_player_ped, true);
					PED::SET_PED_GRAVITY(g_player_ped, true);
				}
				else
				{
					ENTITY::SET_ENTITY_HAS_GRAVITY(g_player_ped, false);
					PED::SET_PED_GRAVITY(g_player_ped, false);
				}
				if (g_player_ped.isOnGround()
					&& !TASK::GET_IS_TASK_ACTIVE(g_player_ped, CTaskTypes::TASK_JUMP)
					)
				{
					PED::SET_PED_CONFIG_FLAG(g_player_ped, 60, true); // on ground
					Vector3 vec = ENTITY::GET_ENTITY_VELOCITY(g_player_ped);
					if (vec.z != 0)
					{
						ENTITY::SET_ENTITY_VELOCITY(g_player_ped, vec.x, vec.y, 0.0f);
					}
				}
				return true;
			});
		}
	};
}
