#pragma once

#include <cstdint>

#include "gta_fwddecl.hpp"

#pragma pack(push, 4)
namespace Stand
{
	template<class T>
	class ReplayInterfaceEntity
	{
	public:
		T* ptr; // may be nullptr
	private:
		char pad[8];
	};
	static_assert(sizeof(ReplayInterfaceEntity<CVehicle>) == 16, "ReplayInterfaceEntity is not sized as expected");

	template<class T, int32_t size>
	class ReplayInterfaceEntityList
	{
	public:
		class ReplayInterfaceEntity<T> entities[size];
	};
}

template<class T, int32_t offset, int32_t list_size>
class CReplayInterface
{
private:
	char pad_0[offset];
public:
	class Stand::ReplayInterfaceEntityList<T, list_size>* list;
	int32_t max;
private:
	char pad[4];
public:
	int32_t cur;
};

class CReplayInterfaceGame
{
private:
	/* 0x00 */ char pad_0x00[0x10 - 0x00];
public:
	/* 0x10 */ class CReplayInterface<CVehicle, 0x180, 300>* veh_interface;
	/* 0x18 */ class CReplayInterface<CPed, 0x100, 256>* ped_interface;
	/* 0x20 */ class CReplayInterface<CPickup, 0x100, 73>* pickup_interface;
	/* 0x28 */ class CReplayInterface<CObject, 0x0158, 2300>* object_interface;
};

namespace Stand
{
	using CReplayInterfaceVehicle = CReplayInterface<CVehicle, 0x180, 300>;
	using CReplayInterfacePed = CReplayInterface<CPed, 0x100, 256>;
	using CReplayInterfacePickup = CReplayInterface<CPickup, 0x100, 73>;
	using CReplayInterfaceObject = CReplayInterface<CObject, 0x0158, 2300>;
}
#pragma pack(pop)
