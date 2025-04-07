#pragma once

#include "natives.hpp"
#include "AbstractEntity.hpp"
#include "CommandAction.hpp"
#include "weapons.hpp"

namespace Stand {
	class CommandFillAmmo : public CommandAction {
	public:
		explicit CommandFillAmmo(CommandList* const parent)
			: CommandAction(parent, LOC("FILAMMO"), { CMDNAME("fillammo") }) {}

		void onClick(Click& click) final
		{
			for (const auto& weapon : Weapon::getAllHashesIncludingWeirdos())
			{
				if(WEAPON::HAS_PED_GOT_WEAPON(g_player_ped, weapon, FALSE))
				{
					WEAPON::ADD_AMMO_TO_PED(g_player_ped, weapon, 9999); // SET_PED_AMMO does not seem to work with special types (incendiary, etc.)
					//WEAPON::SET_AMMO_IN_CLIP(g_player_ped, weapon, 9999); // so we don't have to reload the gun first
				}
			}
			if (auto veh = Util::getVehicle(); veh.isValid())
			{
				for (int i = 0; i < 4; ++i)
				{
					VEHICLE::SET_VEHICLE_WEAPON_RESTRICTED_AMMO(veh, i, -1);
				}
			}
		}
	};
}
