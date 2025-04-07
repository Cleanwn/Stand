#pragma once

#include "atArray.hpp"

class CWeaponObserver : public rage::fwRefAwareBase
{
};
static_assert(sizeof(CWeaponObserver) == 0x10);

#pragma pack(push, 1)
struct CInventoryItem
{
};
//static_assert(sizeof(CInventoryItem) == 0);

struct CWeaponItem /*: public CInventoryItem*/
{
	PAD(0, 0x08) class CWeaponInfo* info;
};
static_assert(sizeof(CWeaponItem) == 0x10);
#pragma pack(pop)

template <typename T>
struct CInventoryItemRepository
{
	rage::atArray<T*> items;
};

struct CWeaponItemRepository : public CInventoryItemRepository<CWeaponItem>
{
};
static_assert(sizeof(CWeaponItemRepository) == sizeof(rage::atArray<CWeaponItem*>));

class CPedInventory : public CWeaponObserver
{
public:
	/* 0x10 */ CPed* ped;
	/* 0x18 */ CWeaponItemRepository weapons;
};
static_assert(sizeof(CPedInventory) == 0x18 + sizeof(CWeaponItemRepository));

