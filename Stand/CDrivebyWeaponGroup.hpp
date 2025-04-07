#pragma once

#include "fwddecl.hpp"
#include "hashtype.hpp"
#include "atArray.hpp"

class CDrivebyWeaponGroup
{
public:
	/* 0x00 */ hash_t m_Name; // e.g. DRIVEBY_DEFAULT_ONE_HANDED
	/* 0x08 */ rage::atArray<hash_t> m_WeaponGroupNames; // GROUP_PISTOL, etc.
	/* 0x18 */ rage::atArray<hash_t> m_WeaponTypeNames; // WEAPON_STUNGUN, etc.

	[[nodiscard]] bool allowsWeapon(const Stand::Weapon& w) const;
};
static_assert(sizeof(CDrivebyWeaponGroup) == 0x28);
