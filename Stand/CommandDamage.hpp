#pragma once

#include "CommandSliderFloat.hpp"

#include "AbstractPlayer.hpp"
#include "natives.hpp"
#include "WeaponMgr.hpp"

namespace Stand
{
	class CommandDamage : public CommandSliderFloat
	{
	public:
		explicit CommandDamage(CommandList* const parent)
			: CommandSliderFloat(parent, LOC("DMG"), { CMDNAME("damagemultiplier") }, LOC("DMG_H"), 0, 1000000, 100, 10)
		{
		}

		void onChange(Click& click, int prev_value) final
		{
			ensureScriptThread(click, [this]
			{
				const float fvalue = (float)value / 100.0f;
				WeaponMgr::weapon_bullet_damage_multiplier = fvalue;
				if (value == 100)
				{
					PLAYER::SET_PLAYER_WEAPON_DAMAGE_MODIFIER(g_player, 1.0f);
					PLAYER::SET_PLAYER_MELEE_WEAPON_DAMAGE_MODIFIER(g_player, 1.0f, TRUE);
					PLAYER::SET_PLAYER_VEHICLE_DAMAGE_MODIFIER(g_player, 1.0f);
				}
				else
				{
					const int value = this->value;
					registerScriptTickEventHandler(TC_SCRIPT_NOYIELD, [this, value, fvalue]()
					{
						if (value != this->value)
						{
							return false;
						}
						PLAYER::SET_PLAYER_WEAPON_DAMAGE_MODIFIER(g_player, fvalue);
						PLAYER::SET_PLAYER_MELEE_WEAPON_DAMAGE_MODIFIER(g_player, fvalue, TRUE);
						PLAYER::SET_PLAYER_VEHICLE_DAMAGE_MODIFIER(g_player, fvalue);
						return true;
					});
				}
			});
		}
	};
}
