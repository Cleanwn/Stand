#include "DrivebyCommons.hpp"

#include "CDrivebyWeaponGroup.hpp"
#include "CVehicleMetadataMgr.hpp"
#include "pointers.hpp"

namespace Stand
{
	void DrivebyCommons::backupDrivebyWeaponGroups()
	{
		if (driveby_weapon_group_backups.empty())
		{
			backupDrivebyWeaponGroupsImpl();
		}
	}

	void DrivebyCommons::backupDrivebyWeaponGroupsImpl()
	{
		for (const auto& group : pointers::vehicle_metadata->driveby_weapon_groups)
		{
			CDrivebyWeaponGroup_Backup backup;
			backup.m_WeaponGroupNames.reserve(group->m_WeaponGroupNames.size());
			for (const auto& wg : group->m_WeaponGroupNames)
			{
				backup.m_WeaponGroupNames.emplace_back(wg);
			}
			SOUP_ASSERT(!driveby_weapon_group_backups.contains(group->m_Name));
			driveby_weapon_group_backups.emplace(group->m_Name, std::move(backup));
		}
	}

	const CDrivebyWeaponGroup_Backup& DrivebyCommons::getDrivebyWeaponGroupBackup(hash_t name)
	{
		return driveby_weapon_group_backups.at(name);
	}

	bool DrivebyCommons::isFakeDrivebyInfo(const CVehicleDriveByInfo* const* ptr)
	{
		for (const auto& addr : fake_driveby_infos)
		{
			if (addr == ptr)
			{
				return true;
			}
		}
		return false;
	}
}
