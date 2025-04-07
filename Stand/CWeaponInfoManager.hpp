#pragma once

#include "atArray.hpp"

class CWeaponInfoBlob
{
	PAD(0, 0xF8);
};
static_assert(sizeof(CWeaponInfoBlob) == 0xF8);

class CWeaponInfoManager
{
	PAD(0, 0x50) rage::atArray<CWeaponInfoBlob> m_WeaponInfoBlobs;
};
//static_assert(offsetof(CWeaponInfoManager, m_WeaponInfoBlobs.m_size) == 0x50 + 8);
/*
BATCH_ADD_OPTIONAL("??", "0F B7 05 ? ? ? ? 33 ED 33 C9", [](soup::Pointer p)
{
	pointers::weapon_info_mgr = p.add(3).rip().sub(0x50 + 8).as<CWeaponInfoManager*>();
});
*/
