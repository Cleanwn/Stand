#pragma once

#include "ropemanager.hpp"

enum eVehicleGadgetType
{
	// networked types
	VGT_FORKS,
	VGT_SEARCHLIGHT,
	VGT_PICK_UP_ROPE, // correct 3095
	VGT_DIGGER_ARM,
	VGT_HANDLER_FRAME,
	VGT_PICK_UP_ROPE_MAGNET, // correct 3095
	VGT_BOMBBAY,
	VGT_TOW_TRUCK_ARM, // correct 3095
	VGT_NUM_NETWORKED_TYPES,

	// non-networked types
	VGT_FIXED_VEHICLE_WEAPON,
	VGT_VEHICLE_WEAPON_BATTERY,
	VGT_TURRET,
	VGT_TURRET_PHYSICAL,
	VGT_TRACKS,
	VGT_LEAN_HELPER,
	VGT_TRAILER_ATTACH_POINT, // correct 3095
	VGT_TRAILER_LEGS,
	VGT_ARTICULATED_DIGGER_ARM,
	VGT_PARKING_SENSOR,
	VGT_THRESHER,
	VGT_BOATBOOM,
	VGT_RADAR,
	VGT_DYNAMIC_SPOILER,
};

class CVehicleGadget
{
public:
	virtual ~CVehicleGadget() = default;
	virtual void ProcessPrePhysics() {}
	virtual void ProcessPhysics() {}
	virtual void ProcessPhysics2() {}
	virtual void ProcessPhysics3() {}
	virtual void ProcessPreRender() {}
	virtual void ProcessPostPreRender() {}
	virtual void ProcessControl() {}
	virtual void Fix() {}
	virtual void ProcessPreComputeImpacts() {}
	virtual int32_t GetType() const = 0; // 0x50, correct 3095
};

class CVehicleGadgetWithJointsBase : public CVehicleGadget
{
};

class CVehicleGadgetTowArm : public CVehicleGadgetWithJointsBase
{
	INIT_PAD(CVehicleGadgetWithJointsBase, 0x88) rage::ropeInstance* m_RopeInstanceA; // correct 3095
	/* 0x90 */ rage::ropeInstance* m_RopeInstanceB; // correct 3095

	void processRopeInstanceBeingRemoved(rage::ropeInstance* pInst)
	{
		if (m_RopeInstanceA == pInst)
		{
			m_RopeInstanceA = nullptr;
		}
		if (m_RopeInstanceB == pInst)
		{
			m_RopeInstanceB = nullptr;
		}
	}
};

class CVehicleGadgetPickUpRope : public CVehicleGadget
{
	INIT_PAD(CVehicleGadgetWithJointsBase, 0x78) rage::ropeInstance* m_RopeInstanceA;
	/* 0x80 */ rage::ropeInstance* m_RopeInstanceB;

	void processRopeInstanceBeingRemoved(rage::ropeInstance* pInst)
	{
		if (m_RopeInstanceA == pInst)
		{
			m_RopeInstanceA = nullptr;
		}
		if (m_RopeInstanceB == pInst)
		{
			m_RopeInstanceB = nullptr;
		}
	}
};

class CVehicleWeapon : public CVehicleGadget
{
public:
	INIT_PAD(CVehicleGadget, 0x10) int32_t m_weaponIndex;
	/* 0x14 */ int32_t m_handlingIndex;
};

class CVehicleWeaponBattery : public CVehicleWeapon
{
public:
	static constexpr int MAX_NUM_BATTERY_WEAPONS = 38;

	enum eMissileFlapState
	{
		MFS_CLOSED,
		MFS_OPEN,
		MFS_CLOSING,
		MFS_OPENING
	};

	enum eMissileBatteryState
	{
		MBS_IDLE,
		MBS_RELOADING
	};

	/* 0x18  */ CVehicleWeapon* m_VehicleWeapons[MAX_NUM_BATTERY_WEAPONS];
	/* 0x148 */ eMissileFlapState m_MissileFlapState[MAX_NUM_BATTERY_WEAPONS];
	/* 0x1E0 */ eMissileBatteryState m_MissileBatteryState;
	/* 0x1E4 */ int32_t m_iLastFiredWeapon;
	/* 0x1E8 */ int32_t m_iNumVehicleWeapons;
	/* 0x1EC */ int32_t m_iCurrentWeaponCounter;
	/* 0x1F0 */ uint32_t m_uNextLaunchTime;
};
static_assert(offsetof(CVehicleWeaponBattery, m_uNextLaunchTime) == 0x1F0);