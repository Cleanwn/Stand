#pragma once

#include "fwddecl.hpp"
#include "hashtype.hpp"
#include "atArray.hpp"

class CVehicleDriveByAnimInfo
{
public:
	/* 0x00 */ hash_t m_Name;
	PAD(0x04, 0x30) CDrivebyWeaponGroup* m_WeaponGroup;
};
static_assert(sizeof(CVehicleDriveByAnimInfo) == 0x30 + 8);

class CVehicleDriveByInfo
{
public:
	/* 0x00 */ hash_t m_Name; // e.g. DRIVEBY_STANDARD_FRONT_LEFT
	PAD(0x04, 0x68) rage::atArray<CVehicleDriveByAnimInfo*> m_DriveByAnimInfos;
};
static_assert(sizeof(CVehicleDriveByInfo) == 0x68 + sizeof(rage::atArray<CVehicleDriveByAnimInfo*>));

class CVehicleSeatInfo
{
};

class CVehicleSeatAnimInfo
{
public:
	/* 0x00 */ hash_t m_Name;
	PAD(0x04, 0x08) CVehicleDriveByInfo* m_DriveByInfo;

	[[nodiscard]] bool allowsWeapon(const Stand::Weapon& w) const;
};
static_assert(sizeof(CVehicleSeatAnimInfo) == 0x10);

class CVehicleLayoutInfo
{
public:
	struct sSeat
	{
		/* 0x00 */ CVehicleSeatInfo* m_SeatInfo;
		/* 0x08 */ CVehicleSeatAnimInfo* m_SeatAnimInfo;
	};
	static_assert(sizeof(sSeat) == 0x10);

	/* 0x00 */ hash_t m_Name; // e.g. LAYOUT_STANDARD
	/* 0x08 */ rage::atArray<sSeat> m_Seats;

	[[nodiscard]] bool doesSeatAllowWeapon(const Stand::Weapon& w, uint16_t seat) const;
	[[nodiscard]] bool doesAnySeatAllowWeapon(const Stand::Weapon& w) const;
};
static_assert(sizeof(CVehicleLayoutInfo) == 0x18);

class CModelSeatInfo
{
	PAD(0x000, 0x348) CVehicleLayoutInfo* m_pLayoutinfo;
};
static_assert(sizeof(CModelSeatInfo) == 0x348 + 8);

class CVehicleModelInfoData
{
public:
	/* 0x000 */ void* m_pStructure;
	/* 0x008 */ CModelSeatInfo m_SeatInfo;
};
static_assert(sizeof(CVehicleModelInfoData) == 0x008 + sizeof(CModelSeatInfo));

static_assert(offsetof(CVehicleModelInfoData, m_SeatInfo.m_pLayoutinfo) == 0x350);
