#pragma once

#include "CommandToggle.hpp"

#include "AbstractEntity.hpp"
#include "gta_input.hpp"
#include "input.hpp"
#include "natives.hpp"

namespace Stand
{
	class CommandRapidFire : public CommandToggle
	{
	public:
		explicit CommandRapidFire(CommandList* const parent)
			: CommandToggle(parent, LOC("RAPFRE"), { CMDNAME("rapidfire") })
		{
		}

		void onChange(Click& click) final
		{
			onChangeToggleScriptTickEventHandler(click, []()
			{
				if (Input::isPressingAttack())
				{
					Hash weaponHash;
					if (WEAPON::GET_CURRENT_PED_WEAPON(g_player_ped, &weaponHash, true))
					{
						v3 start_pos;
						if (shouldUseMuzzlePos(weaponHash))
						{
							start_pos = g_player_ped.getWeaponMuzzlePos();
							if (start_pos.isNull()) // Throwables
							{
								start_pos = g_player_ped.getWeapon().getPos();
							}
						}
						else
						{
							start_pos = CAM::GET_FINAL_RENDERED_CAM_COORD();
						}
						const auto target_pos = CAM::GET_FINAL_RENDERED_CAM_COORD() + (CAM::GET_FINAL_RENDERED_CAM_ROT(2).toDir() * WEAPON::GET_MAX_RANGE_OF_CURRENT_PED_WEAPON(g_player_ped));
						//GRAPHICS::DRAW_LINE(start_pos.x, start_pos.y, start_pos.z, target_pos.x, target_pos.y, target_pos.z, 255, 0, 255, 255);
						MISC::SHOOT_SINGLE_BULLET_BETWEEN_COORDS_IGNORE_ENTITY(start_pos.x, start_pos.y, start_pos.z, target_pos.x, target_pos.y, target_pos.z, 250, true, weaponHash, g_player_ped, true, false, -1.0f, g_player_ped, 0);
					}
				}
			});
		}

		[[nodiscard]] static bool shouldUseMuzzlePos(hash_t weaponHash)
		{
			if (weaponHash == ATSTRINGHASH("WEAPON_GRENADELAUNCHER")
				|| weaponHash == ATSTRINGHASH("WEAPON_COMPACTLAUNCHER")
				|| weaponHash == ATSTRINGHASH("WEAPON_EMPLAUNCHER")
				|| weaponHash == ATSTRINGHASH("WEAPON_SNOWLAUNCHER")
				)
			{
				return true;
			}
			if (auto w = Weapon::find(weaponHash))
			{
				return w->category == Weapon::THROWABLE;
			}
			return false;
		}
	};
}
