#pragma once

#include "CommandToggle.hpp"

#include "atArray.hpp"
#include "CDrivebyWeaponGroup.hpp"
#include "CVehicleMetadataMgr.hpp"
#include "DrivebyCommons.hpp"

namespace Stand
{
	class CommandDrivebyWithAny : public CommandToggle
	{
	public:
		explicit CommandDrivebyWithAny(CommandList* const parent)
			: CommandToggle(parent, LOC("DRBYWITHANY"), CMDNAMES("drivebywithall", "drivebywithany", "drivebyallguns"))
		{
		}

		void onEnable(Click& click) final
		{
			ensureScriptThread(click, [this]
			{
				DrivebyCommons::backupDrivebyWeaponGroups();
				for (const auto& group : pointers::vehicle_metadata->driveby_weapon_groups)
				{
					constexpr Hash groups[] = {
						ATSTRINGHASH("GROUP_SHOTGUN"),
						ATSTRINGHASH("GROUP_SNIPER"),
						ATSTRINGHASH("GROUP_HEAVY"),
						ATSTRINGHASH("GROUP_PISTOL"),
						ATSTRINGHASH("GROUP_SMG"),
						ATSTRINGHASH("GROUP_STUNGUN"),
						ATSTRINGHASH("GROUP_RIFLE"),
						ATSTRINGHASH("GROUP_MG"),
						ATSTRINGHASH("GROUP_MELEE"), // Kind of useless, you aimlessly flip people off with the weapon in your hand (or do nothing at all). 
						ATSTRINGHASH("GROUP_FIREEXTINGUISHER"), // Kind of useless, you aimlessly point it into the air.
						ATSTRINGHASH("GROUP_PETROLCAN"), // Kind of useless, you aimlessly pour it on your own vehicle.

						// Missing groups:
						//  1. GROUP_METALDETECTOR isn't a legitimate weapon.
						//  2. GROUP_UNARMED encompasses WEAPON_UNARMED & WEAPON_KNUCKLE. Driveby with Knuckle Duster crashes the game.
						//  3. GROUP_THROWN is not present in m_WeaponGroupNames by default. Inserting it will soft-lock the game when a throwable is aimed from a vehicle, regardless of whether the vehicle supports throwables.
						//
						// This feature does not work for a Nightshark with window armour upgrades. The GROUP_THROWN weirdness combined with the Nightshark weirdness suggests the game has scattered logic responsible for drivebys.
					};

					for (const auto hash : groups)
					{
						if (group->m_WeaponGroupNames.Find(hash) == -1)
						{
							group->m_WeaponGroupNames.emplace_back(hash);
						}
					}
				}
			});
		}

		void onDisable(Click& click) final
		{
			ensureScriptThread(click, [this]
			{
				for (const auto& group : pointers::vehicle_metadata->driveby_weapon_groups)
				{
					const std::vector<hash_t>& og_weapon_group_names = DrivebyCommons::getDrivebyWeaponGroupBackup(group->m_Name).m_WeaponGroupNames;
					group->m_WeaponGroupNames.clear();
					if (!og_weapon_group_names.empty())
					{
						group->m_WeaponGroupNames.reserve((uint16_t)og_weapon_group_names.size());
						for (const auto& wg : og_weapon_group_names)
						{
							group->m_WeaponGroupNames.emplace_back(wg);
						}
					}
				}
			});
		}
	};
}
