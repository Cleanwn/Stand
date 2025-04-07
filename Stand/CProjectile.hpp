#pragma once

#include "CObject.hpp"

class CProjectile : public CObject
{
public:
	// rtti shim 1 (0x00)
	// rtti shim 2 (0x08)
	// rtti shim 3 (0x10)
	// rtti shim 4 (0x18)
	// rtti shim 5 (0x20)
	// rtti shim 6 (0x28)
	virtual ~CProjectile() = default; // 0x30
	virtual void _038() = 0;
	virtual void _040() = 0;
	virtual void _048() = 0;
	virtual void _050() = 0;
	virtual void _058() = 0;
	virtual void _060() = 0;
	virtual void _068() = 0;
	virtual void _070() = 0;
	virtual void _078() = 0;
	virtual void _080() = 0;
	virtual void _088() = 0;
	virtual void _090() = 0;
	virtual void _098() = 0;
	virtual void _0A0() = 0;
	virtual void _0A8() = 0;
	virtual void _0B0() = 0;
	virtual void _0B8() = 0;
	virtual void _0C0() = 0;
	virtual void _0C8() = 0;
	virtual void _0D0() = 0;
	virtual void _0D8() = 0;
	virtual void _0E0() = 0;
	virtual void _0E8() = 0;
	virtual void _0F0() = 0;
	virtual void _0F8() = 0;
	virtual void _100() = 0;
	virtual void _108() = 0;
	virtual void _110() = 0;
	virtual void _118() = 0;
	virtual void _120() = 0;
	virtual void _128() = 0;
	virtual void _130() = 0;
	virtual void _138() = 0;
	virtual void _140() = 0;
	virtual void _148() = 0;
	virtual void _150() = 0;
	virtual void _158() = 0;
	virtual void _160() = 0;
	virtual void _168() = 0;
	virtual void _170() = 0;
	virtual void _178() = 0;
	virtual void _180() = 0;
	virtual void _188() = 0;
	virtual void _190() = 0;
	virtual void _198() = 0;
	virtual void _1A0() = 0;
	virtual void _1A8() = 0;
	virtual void _1B0() = 0;
	virtual void _1B8() = 0;
	virtual void _1C0() = 0;
	virtual void _1C8() = 0;
	virtual void _1D0() = 0;
	virtual void _1D8() = 0;
	virtual void _1E0() = 0;
	virtual void _1E8() = 0;
	virtual void _1F0() = 0;
	virtual void _1F8() = 0;
	virtual void _200() = 0;
	virtual void _208() = 0;
	virtual void _210() = 0;
	virtual void _218() = 0;
	virtual void _220() = 0;
	virtual void _228() = 0;
	virtual void _230() = 0;
	virtual void _238() = 0;
	virtual void _240() = 0;
	virtual void _248() = 0;
	virtual void _250() = 0;
	virtual void _258() = 0;
	virtual void _260() = 0;
	virtual void _268() = 0;
	virtual void _270() = 0;
	virtual void _278() = 0;
	virtual void _280() = 0;
	virtual void _288() = 0;
	virtual void _290() = 0;
	virtual void _298() = 0;
	virtual void _2A0() = 0;
	virtual void _2A8() = 0;
	virtual void _2B0() = 0;
	virtual void _2B8() = 0;
	virtual void _2C0() = 0;
	virtual void _2C8() = 0;
	virtual void _2D0() = 0;
	virtual void _2D8() = 0;
	virtual void _2E0() = 0;
	virtual void _2E8() = 0;
	virtual void _2F0() = 0;
	virtual void _2F8() = 0;
	virtual void _300() = 0;
	virtual void _308() = 0;
	virtual void _310() = 0;
	virtual void _318() = 0;
	virtual void _320() = 0;
	virtual void _328() = 0;
	virtual void _330() = 0;
	virtual void _338() = 0;
	virtual void _340() = 0;
	virtual void _348() = 0;
	virtual void _350() = 0;
	virtual void _358() = 0;
	virtual void _360() = 0;
	virtual void _368() = 0;
	virtual void _370() = 0;
	virtual void _378() = 0;
	virtual void _380() = 0;
	virtual void _388() = 0;
	virtual void _390() = 0;
	virtual void _398() = 0;
	virtual void _3A0() = 0;
	virtual void _3A8() = 0;
	virtual void _3B0() = 0;
	virtual void _3B8() = 0;
	virtual void _3C0() = 0;
	virtual void _3C8() = 0;
	virtual void _3D0() = 0;
	virtual void _3D8() = 0;
	virtual void _3E0() = 0;
	virtual void _3E8() = 0;
	virtual void _3F0() = 0;
	virtual void _3F8() = 0;
	virtual void _400() = 0;
	virtual void _408() = 0;
	virtual void _410() = 0;
	virtual void _418() = 0;
	virtual void _420() = 0;
	virtual void _428() = 0;
	virtual void _430() = 0;
	virtual void _438() = 0;
	virtual void _440() = 0;
	virtual void _448() = 0;
	virtual void _450() = 0;
	virtual void _458() = 0;
	virtual void _460() = 0;
	virtual void _468() = 0;
	virtual void _470() = 0;
	virtual void _478() = 0;
	virtual void _480() = 0;
	virtual void _488() = 0;
	virtual void _490() = 0;
	virtual void _498() = 0;
	virtual void _4A0() = 0;
	virtual void _4A8() = 0;
	virtual void _4B0() = 0;
	virtual void _4B8() = 0;
	virtual void _4C0() = 0;
	virtual void _4C8() = 0;
	virtual void _4D0() = 0;
	virtual void Destroy() = 0; // 0x4D8
	virtual void Explode(const rage::Vector3*, const rage::Vector3*, const CEntity*, bool, const unsigned int) = 0; // 0x4E0, this works fine for stationary projectiles, but moving projectiles cause glitchy explosions. Prefer CProjectile::TriggerExplosion to prevent that.

	INIT_PAD(CObject, 0x4E0) uint32_t m_uWeaponFiredFromHash;
	PAD(0x4E0 + 4, 0x4E8) CEntity* owner;
	PAD(0x4E8 + 8, 0x4F4) float age;
	PAD(0x4F4 + 4, 0x5C3) bool m_bProximityMineTriggered;
};
static_assert(offsetof(CProjectile, age) == 0x4F4);
static_assert(offsetof(CProjectile, m_bProximityMineTriggered) == 0x5C3);
