#pragma once

#include "gta_entity.hpp"

class CObject : public CPhysical
{
	INIT_PAD(CPhysical, 0x2F8) uint8_t object_flags; // bitflag; 0b1 = pickup, 0b100 = dynamic
	PAD(0x2F8 + 1, 0x320) CWeapon* m_pWeapon; // 48 8B 97 ? ? 00 00 4C 8B C3 49 8B CE E8 ? ? ? ? 48 8B F0 48 85 C0
};
static_assert(sizeof(CObject) == 0x320 + 8);
