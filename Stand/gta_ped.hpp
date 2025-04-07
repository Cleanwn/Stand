#pragma once

#include "struct_base.hpp"

#include <soup/BigBitset.hpp>

#include "fwRefAwareBase.hpp"
#include "fwRegdRef.hpp"
#include "gta_entity.hpp"
#include "vector.hpp"

#pragma pack(push, 1)
class CPedFactory
{
public:
	virtual ~CPedFactory() = default;
	virtual void _0x08() = 0;
	virtual void _0x10() = 0;
	virtual void _0x18() = 0;
	virtual void _0x20() = 0;
	virtual void _0x28() = 0;
	virtual void deletePed(CPed*, bool) = 0;

	/* 0x08 */ class CPed* m_local_ped;
};
static_assert(sizeof(CPedFactory) == 0x10);

class CInventoryListener : public rage::fwRefAwareBase
{
};

class audWeaponInventoryListener : public CInventoryListener
{
};

class CPedClothCollision : public rage::datBase
{
};

struct CPedConfigFlags
{
	PAD(0, 0x08) soup::BigBitset<60> m_Flags;
};

struct CPedResetFlags
{
	PAD(0, 0x20) soup::BigBitset<60> m_Flags;
};

class CPed : public CPhysical
{
	INIT_PAD(CPhysical, 0xD10) CVehicle* current_or_last_vehicle;
	PAD(0xD10 + 8, 0xDC0) audWeaponInventoryListener weapon_inventory_listener;
	PAD(0xDC0 + sizeof(audWeaponInventoryListener), 0xE32) bool is_in_vehicle;
	PAD(0xE32 + 1, 0x10A0) class CPedIntelligence* intelligence;
	/* 0x10A8 */ class CPlayerInfo* player_info;
	/* 0x10B0 */ class CPedInventory* inventory;
	/* 0x10B8 */ class CPedWeaponManager* weapon_manager;
	PAD(0x10B8 + 8, 0x13C8) CPedClothCollision* cloth_collision; // possibly outdated offset
	PAD(0x13C8 + 8, 0x143C) CPedConfigFlags m_PedConfigFlags;
	/* 0x1480 */ CPedResetFlags m_PedResetFlags;
	PAD(0x1480 + sizeof(CPedResetFlags), 0x150C) float m_armor;
	PAD(0x150C + 4, 0x1530) rage::fwRegdRef<CVehicle> m_pMyVehicle;
	PAD(0x1530 + 8, 0x1964) uint8_t m_uStickyCount; // 74 ? 48 83 C4 20 5B E9 ? ? ? ? 48 83 C4 20 5B E9
	/* 0x1965 */ uint8_t m_uStickToPedProjCount;
	/* 0x1966 */ uint8_t m_uFlareGunProjCount;

	[[nodiscard]] bool IsAPlayerPed() const noexcept
	{
		return player_info != nullptr;
	}

	[[nodiscard]] CVehicle* getVehicle(bool include_last = false) const noexcept
	{
		if (include_last
			|| is_in_vehicle // The game would use CPED_CONFIG_FLAG_InVehicle for this
			)
		{
			return current_or_last_vehicle;
		}
		return nullptr;
	}
};
static_assert(sizeof(CPed) == 0x1966 + 1);
static_assert(offsetof(CPed, player_info) == 0x10A8); // 2944

class CWeapon : public rage::fwRefAwareBase
{
	INIT_PAD(rage::fwRefAwareBase, 0x20) rage::Vec3V m_muzzlePos;
	PAD(0x30, 0x58) rage::fwRegdRef<CDynamicEntity> m_pDrawableEntity;
	PAD(0x60, 0x178) rage::fwRegdRef<CEntity> m_pMuzzleEntity;
};
static_assert(offsetof(CWeapon, m_muzzlePos) == 0x20);
static_assert(offsetof(CWeapon, m_pDrawableEntity) == 0x58);
static_assert(offsetof(CWeapon, m_pMuzzleEntity) == 0x178);

struct CWeaponModelInfo : public CBaseModelInfo
{
	/* 0xB0 */ int32_t m_nBoneIndices[/* MAX_WEAPON_NODES */ 36];
};

class CPedWeaponSelector
{
public:
	/* 0x00 */ hash_t selected_weapon_hash;
	PAD(0x04, 0x08) class CWeaponInfo* selected_weapon_info;
	PAD(0x10, 0x40);
};
static_assert(sizeof(CPedWeaponSelector) == 0x40);

class CPedEquippedWeapon
{
	/* 0x00 */ class CPed* ped;
	/* 0x08 */ bool weaponAnimsRequested;
	/* 0x09 */ bool weaponAnimsStreamed;
	PAD(0x0A, 0x10) class CWeaponInfo* equippedWeaponInfo;
	/* 0x18 */ class CWeaponInfo* equippedVehicleWeaponInfo;
	/* 0x20 */ rage::fwRegdRef<CObject> object;
	/* 0x28 */ rage::fwRegdRef<CObject> secondObject;
	/* 0x30 */ rage::fwRegdRef<CWeapon> unarmedWeapon;
	/* 0x38 */ rage::fwRegdRef<CWeapon> unarmedKungFuWeapon;
	/* 0x40 */ rage::fwRegdRef<CWeapon> weaponNoModel;
};
static_assert(sizeof(CPedEquippedWeapon) == 0x40 + 8);

class CPedWeaponManager : public CInventoryListener
{
public:
	/* 0x10 */ class CPed* ped;
	/* 0x18 */ CPedWeaponSelector selector;
	/* 0x58 */ CPedEquippedWeapon equippedWeapon;
	
	[[nodiscard]] const CWeapon* GetEquippedWeapon() const;
};
static_assert(sizeof(CPedWeaponManager) == 0x58 + sizeof(CPedEquippedWeapon));

class CItemInfo : public rage::fwRefAwareBase
{
public:
	/* 0x10 */ hash_t name_hash;
	/* 0x14 */ hash_t model_hash; // e.g. w_lr_homing_rocket
	/* 0x18 */ hash_t audio_hash;
	/* 0x1C */ hash_t slot_hash;
};
static_assert(sizeof(CItemInfo) == 0x20);

class CWeaponInfo : public CItemInfo
{
	INIT_PAD(CItemInfo, 0x060) class CAmmoInfo* m_AmmoInfo;
	PAD(0x060 + 8, 0x074) float m_AccuracySpread;
	/* 0x078 */ float m_AccurateModeAccuracyModifier;
	/* 0x07c */ float m_RunAndGunAccuracyModifier;
	/* 0x080 */ float m_RunAndGunAccuracyMinOverride;
	/* 0x084 */ float m_RecoilAccuracyMax;
	/* 0x088 */ float m_RecoilErrorTime;
	/* 0x08c */ float m_RecoilRecoveryRate;
	/* 0x090 */ float m_RecoilAccuracyToAllowHeadShotAI;
	/* 0x094 */ float m_MinHeadShotDistanceAI;
	/* 0x098 */ float m_MaxHeadShotDistanceAI;
	/* 0x09c */ float m_HeadShotDamageModifierAI;
	/* 0x0a0 */ float m_RecoilAccuracyToAllowHeadShotPlayer;
	/* 0x0a4 */ float m_MinHeadShotDistancePlayer;
	/* 0x0a8 */ float m_MaxHeadShotDistancePlayer;
	/* 0x0ac */ float m_HeadShotDamageModifierPlayer;
	/* 0x0b0 */ float m_Damage;
	/* 0x0b4 */ float m_DamageTime;
	/* 0x0b8 */ float m_DamageTimeInVehicle;
	/* 0x0bc */ float m_DamageTimeInVehicleHeadShot;
	PAD(0x0bc + 4, 0x0f8) float m_ForceMaxStrengthMult;
	/* 0x0fc */ float m_ForceFalloffRangeStart;
	/* 0x100 */ float m_ForceFalloffRangeEnd;
	/* 0x104 */ float m_ForceFalloffMin;
	/* 0x108 */ float m_ProjectileForce;
	/* 0x10c */ float m_FragImpulse;
	/* 0x110 */ float m_Penetration;
	/* 0x114 */ float m_VerticalLaunchAdjustment;
	/* 0x118 */ float m_DropForwardVelocity;
	/* 0x11c */ float m_Speed;
	/* 0x120 */ unsigned m_BulletsInBatch;
	/* 0x124 */ float m_BatchSpread;
	/* 0x128 */ float m_ReloadTimeMP;
	/* 0x12c */ float m_ReloadTimeSP;
	/* 0x130 */ float m_VehicleReloadTime;
	/* 0x134 */ float m_AnimReloadRate;
	/* 0x138 */ int m_BulletsPerAnimLoop;
	/* 0x13c */ float m_TimeBetweenShots;
	/* 0x140 */ float m_TimeLeftBetweenShotsWhereShouldFireIsCached;
	/* 0x144 */ float m_SpinUpTime;
	/* 0x148 */ float m_SpinTime;
	/* 0x14c */ float m_SpinDownTime;
	/* 0x150 */ float m_AlternateWaitTime;
	/* 0x154 */ float m_BulletBendingNearRadius;
	/* 0x158 */ float m_BulletBendingFarRadius;
	/* 0x15c */ float m_BulletBendingZoomedRadius;
	/* 0x160 */ float m_FirstPersonBulletBendingNearRadius;
	/* 0x164 */ float m_FirstPersonBulletBendingFarRadius;
	/* 0x168 */ float m_FirstPersonBulletBendingZoomedRadius;
	PAD(0x168 + 4, 0x250) int m_InitialRumbleDuration;
	/* 0x254 */ float m_InitialRumbleIntensity;
	/* 0x258 */ float m_InitialRumbleIntensityTrigger;
	/* 0x25c */ int m_RumbleDuration;
	/* 0x260 */ float m_RumbleIntensity;
	/* 0x264 */ float m_RumbleIntensityTrigger;
	/* 0x268 */ float m_RumbleDamageIntensity;
	/* 0x26c */ int m_InitialRumbleDurationFps;
	/* 0x270 */ float m_InitialRumbleIntensityFps;
	/* 0x274 */ int m_RumbleDurationFps;
	/* 0x278 */ float m_RumbleIntensityFps;
	/* 0x27c */ float m_NetworkPlayerDamageModifier;
	/* 0x280 */ float m_NetworkPedDamageModifier;
	/* 0x284 */ float m_NetworkHeadShotPlayerDamageModifier;
	/* 0x288 */ float m_LockOnRange;
	/* 0x28c */ float m_WeaponRange;
	/* 0x290 */ float m_AiSoundRange;
	/* 0x294 */ float m_AiPotentialBlastEventRange;
	/* 0x298 */ float m_DamageFallOffRangeMin;
	/* 0x29c */ float m_DamageFallOffRangeMax;
	PAD(0x29c + 4, 0x2a8) float m_DamageFallOffModifier;
	PAD(0x2a8 + 4, 0x2e4) hash_t m_RecoilShakeHash;
	/* 0x2e8 */ hash_t m_RecoilShakeHashFirstPerson;
	/* 0x2ec */ hash_t m_AccuracyOffsetShakeHash;
	/* 0x2f0 */ unsigned int m_MinTimeBetweenRecoilShakes;
	/* 0x2f4 */ float m_RecoilShakeAmplitude;
	/* 0x2f8 */ float m_ExplosionShakeAmplitude;
	PAD(0x2f8 + 4, 0x8fc) float m_BulletDirectionOffsetInDegrees;
	/* 0x900 */ float m_BulletDirectionPitchOffset;
	/* 0x904 */ float m_BulletDirectionPitchHomingOffset;
};
static_assert(sizeof(CWeaponInfo) == 0x904 + 4);

class CAmmoInfo : public CItemInfo
{
public:
	/* 0x20 */ int m_AmmoMax;
	/* 0x24 */ int m_AmmoMax50;
	/* 0x28 */ int m_AmmoMax100;
	/* 0x2c */ int m_AmmoMaxMP;
	/* 0x30 */ int m_AmmoMax50MP;
	/* 0x34 */ int m_AmmoMax100MP;
};
static_assert(sizeof(CAmmoInfo) == 0x34 + 4);

class CAmmoProjectileInfo : public CAmmoInfo
{
public:
	class sHomingRocketParams
	{
	public:
		/* 0x00 */ bool ShouldUseHomingParamsFromInfo;
		/* 0x01 */ bool ShouldIgnoreOwnerCombatBehaviour;
		/* 0x04 */ float TimeBeforeStartingHoming;
		/* 0x08 */ float TimeBeforeHomingAngleBreak;
		/* 0x0C */ float TurnRateModifier;
		/* 0x10 */ float PitchYawRollClamp;
		/* 0x14 */ float DefaultHomingRocketBreakLockAngle;
		/* 0x18 */ float DefaultHomingRocketBreakLockAngleClose;
		/* 0x1C */ float DefaultHomingRocketBreakLockCloseDistance;
	};

	INIT_PAD(CAmmoInfo, 0x58) float m_LaunchSpeed;
};
static_assert(sizeof(CAmmoProjectileInfo) == 0x58 + 4);

class CAmmoRocketInfo : public CAmmoProjectileInfo
{
	INIT_PAD(CAmmoProjectileInfo, 0x178) float m_TimeBeforeHoming;
	/* 0x17C */ float m_TimeBeforeSwitchTargetMin;
	/* 0x180 */ float m_TimeBeforeSwitchTargetMax;
	/* 0x184 */ float m_ProximityRadius;
	/* 0x188 */ float m_PitchChangeRate;
	/* 0x18C */ float m_YawChangeRate;
	/* 0x190 */ float m_RollChangeRate;
	/* 0x194 */ float m_MaxRollAngleSin;
	/* 0x198 */ float m_LifeTimePlayerVehicleLockedOverrideMP;
	/* 0x19C */ CAmmoProjectileInfo::sHomingRocketParams m_HomingRocketParams;
};
static_assert(sizeof(CAmmoRocketInfo) == 0x19C + sizeof(CAmmoProjectileInfo::sHomingRocketParams));
#pragma pack(pop)
