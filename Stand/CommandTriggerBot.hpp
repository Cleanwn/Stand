#pragma once

#include "CommandToggle.hpp"

#include "AbstractEntity.hpp"
#include "eDamageFlags.hpp"
#include "natives.hpp"
#include "TargetingData.hpp"

namespace Stand
{
	class CommandTriggerBot : public CommandToggle
	{
	public:
		bool magic_bullets = false;
		TargetingData targeting{};

		explicit CommandTriggerBot(CommandList* const parent)
			: CommandToggle(parent, LOC("TRIGBT"), CMDNAMES("triggerbot"))
		{
		}

		void onEnable(Click& click) final
		{
			registerScriptTickEventHandler(click, [this]()
			{
				if (!PED::IS_PED_RELOADING(g_player_ped)
					&& WEAPON::IS_PED_WEAPON_READY_TO_SHOOT(g_player_ped)
					)
				{
					Entity ent{};

					if (PLAYER::GET_ENTITY_PLAYER_IS_FREE_AIMING_AT(g_player, &ent))
					{
						if (auto entity = AbstractEntity::get(ent); entity.canTarget(targeting, true, magic_bullets))
						{
							PAD::SET_CONTROL_VALUE_NEXT_FRAME(0, INPUT_ATTACK, 1.0f);
							PAD::SET_CONTROL_VALUE_NEXT_FRAME(0, INPUT_VEH_ATTACK, 1.0f);
							PAD::SET_CONTROL_VALUE_NEXT_FRAME(0, INPUT_VEH_PASSENGER_ATTACK, 1.0f);
							PAD::SET_CONTROL_VALUE_NEXT_FRAME(0, INPUT_VEH_FLY_ATTACK, 1.0f);
							PAD::SET_CONTROL_VALUE_NEXT_FRAME(0, INPUT_ATTACK2, 1.0f);

							if (magic_bullets
								&& entity.isAPlayer()
								&& (!g_player_veh.isValid() || entity.getVehicle() != g_player_veh)
								)
							{
								const auto weapon = WEAPON::GET_SELECTED_PED_WEAPON(g_player_ped);
								const auto damage = WEAPON::GET_WEAPON_DAMAGE(weapon, 0);

								entity.sendMagicBullet(weapon, damage, true);
							}
						}
					}
				}
				return m_on;
			});
		}
	};
}
