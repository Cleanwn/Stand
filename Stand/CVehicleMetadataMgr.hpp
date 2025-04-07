#pragma once

#include "gta_fwddecl.hpp"

struct CVehicleMetadataMgr
{
	PAD(0x00, 0xA0) rage::atArray<CDrivebyWeaponGroup*> driveby_weapon_groups;
	PAD(0xB0, 0xC0) rage::atArray<CVehicleDriveByInfo*> driveby_infos;
};
