#pragma once

#include "CommandToggle.hpp"

#include "gta_ped.hpp"
#include "natives.hpp"
#include "weapons.hpp"

namespace Stand
{
	class CommandInfiniteAmmo : public CommandToggle
	{
	public:
		explicit CommandInfiniteAmmo(CommandList* const parent)
			: CommandToggle(parent, LOC("INFAMMO"), CMDNAMES("bottomless", "botomless", "infammo", "infiniteammo"))
		{
		}

		void onChange(Click& click) final
		{
			onChangeToggleScriptTickEventHandler(click, []()
			{
				if (auto* const ped = g_player_ped.getCPed())
				{
					if (CPedWeaponManager* const weapon_manager = ped->weapon_manager)
					{
						if (CWeaponInfo* const current_weapon = weapon_manager->equippedWeapon.equippedWeaponInfo)
						{
							if (CAmmoInfo* const ammo_info = current_weapon->m_AmmoInfo)
							{
								const Hash weaponHash = WEAPON::GET_SELECTED_PED_WEAPON(g_player_ped);
								int max_ammo = ammo_info->m_AmmoMax100;
								const auto weapon = Weapon::find(weaponHash);
								if (weapon != nullptr && weapon->category == Weapon::THROWABLE)
								{
									max_ammo += 5;
								}
								WEAPON::ADD_AMMO_TO_PED(g_player_ped, weaponHash, max_ammo); // SET_PED_AMMO does not seem to work with special types (incendiary, etc.)
							}
						}
					}
				}
				if (auto veh = Util::getVehicle(); veh.isValid())
				{
					for (int i = 0; i < 4; ++i)
					{
						VEHICLE::SET_VEHICLE_WEAPON_RESTRICTED_AMMO(veh, i, -1);
					}
				}
			});
		}
	};
}
