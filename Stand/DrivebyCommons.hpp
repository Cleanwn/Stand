#pragma once

#include <unordered_map>
#include <vector>

#include "gta_fwddecl.hpp"
#include "hashtype.hpp"

namespace Stand
{
	struct CDrivebyWeaponGroup_Backup
	{
		std::vector<hash_t> m_WeaponGroupNames;
	};

	class DrivebyCommons
	{
	private:
		inline static std::unordered_map<hash_t, CDrivebyWeaponGroup_Backup> driveby_weapon_group_backups;

	public:
		static void backupDrivebyWeaponGroups();
	private:
		static void backupDrivebyWeaponGroupsImpl();

	public:
		[[nodiscard]] static const CDrivebyWeaponGroup_Backup& getDrivebyWeaponGroupBackup(hash_t name);

		inline static std::vector<CVehicleDriveByInfo**> fake_driveby_infos;

		[[nodiscard]] static bool isFakeDrivebyInfo(const CVehicleDriveByInfo* const* ptr);
	};
}
