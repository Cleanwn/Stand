#pragma once

#include "struct_base.hpp"
#include "fwRefAwareBase.hpp"
#include "fwRegdRef.hpp"

#pragma pack(push, 1)
namespace rage
{
	class aiTask : public fwRefAwareBase
	{
	public:
		virtual ~aiTask() = default;
		virtual void _0x08() = 0;
		virtual void _0x10() = 0;

		enum AbortPriority
		{
			ABORT_PRIORITY_URGENT = 0,
			ABORT_PRIORITY_IMMEDIATE
		};

		virtual void MakeAbortable(AbortPriority priority, void* aiEvent = nullptr) = 0;

		/* 0x10 */ fwEntity* entity;
		/* 0x18 */ aiTask* parent;
		/* 0x20 */ aiTask* sub_task;
		PAD(0x28, 0x30) uint16_t flags;
		PAD(0x32, 0x34) uint16_t type_id;
		/* 0x36 */ uint8_t state;
		PAD(0x36 + 1, 0x40);

		void setHasFinishedFlag() noexcept
		{
			flags |= 2;
		}
	};
	static_assert(sizeof(aiTask) == 0x40);
}

class CTask : public rage::aiTask
{
public:
	virtual ~CTask() = default;
	// _0x08
	// _0x10
	// MakeAbortable
	virtual void _0x20() = 0;
	virtual void _0x28() = 0;
	virtual void _0x30() = 0;
	virtual void _0x38() = 0;
	virtual void _0x40() = 0;
	virtual void _0x48() = 0;
	virtual void _0x50() = 0;
	virtual void _0x58() = 0;
	virtual void _0x60() = 0;
	virtual void _0x68() = 0;
	virtual void _0x70() = 0;
	virtual void _0x78() = 0;
	virtual void _0x80() = 0;
	virtual void _0x88() = 0;
	virtual void _0x90() = 0;
	virtual void _0x98() = 0;
	virtual void _0xA0() = 0;
	virtual void _0xA8() = 0;
	virtual void _0xB0() = 0;
	virtual void _0xB8() = 0;
	virtual void _0xC0() = 0;
	virtual void _0xC8() = 0;
	virtual void _0xD0() = 0;
	virtual void _0xD8() = 0;
	virtual void _0xE0() = 0;
	virtual void _0xE8() = 0;
	virtual void _0xF0() = 0;
	virtual void _0xF8() = 0;
	virtual void _0x100() = 0;
	virtual bool IsNMBehaviourTask() const noexcept = 0;
	virtual bool IsBlendFromNMTask() const noexcept = 0;

	bool IsValidNMControlSubTask() const noexcept;

	INIT_PAD(rage::aiTask, 0x58);
};
static_assert(sizeof(CTask) == 0x58);

// FSM = Finite State Machine
class CTaskFSMClone : public CTask
{
	INIT_PAD(CTask, 0xA1);
};
static_assert(sizeof(CTaskFSMClone) == 0xA1);

class CTaskFall : public CTaskFSMClone
{
	INIT_PAD(CTaskFSMClone, 0x180) uint32_t flags;
};
static_assert(sizeof(CTaskFall) == 0x180 + 4);

class CTaskTakeOffPedVariation : public CTaskFSMClone
{
	INIT_PAD(CTaskFSMClone, 0x108) CObject* object;
	PAD(0x110, 0x124) int32_t object_model; // e.g. p_parachute_s
};
static_assert(sizeof(CTaskTakeOffPedVariation) == 0x124 + 4);

class CTaskInVehicleSeatShuffle : public CTaskFSMClone
{
	INIT_PAD(CTaskFSMClone, 0x120) rage::fwRegdRef<CVehicle> m_pVehicle;
	/* 0x128 */ rage::fwRegdRef<CVehicle> m_pJackedPed;
	/* 0x130 */ int32_t m_iTargetSeatIndex;
	/* 0x134 */ int32_t m_iCurrentSeatIndex;
};

class CTaskVehicleFSM : public CTaskFSMClone
{
	INIT_PAD(CTaskFSMClone, 0xB8) rage::fwRegdRef<CVehicle> m_pVehicle;
	PAD(0xB8 + 8, 0xCC) int m_iSeat; // Driver = 0, Passenger Side = 1. Offset by +1.
	PAD(0xCC + 4, 0xD8) bool m_bWarping; // If they need to teleport to enter.
	PAD(0xD8 + 1, 0xDA) bool m_bWaitForSeatToBeOccupied;
};

class CTaskEnterVehicle : public CTaskVehicleFSM
{
	INIT_PAD(CTaskVehicleFSM, 0x1E0) rage::fwRegdRef<CPed const> m_pJackedPed; // Set when doing legitimate jacks, during the "tearout" animation.
	PAD(0x1E8 + 8, 0x250) rage::Vector3 m_targetPosition; // Seems like nonsense. The distance is always +600.
	/* 0x260 */ rage::fwRegdRef<CPed const> m_pTargetPed;
	PAD(0x260 + 8, 0x26D) bool m_bLerpingCloneToDoor;
};

class CVehicleMissionBase : public CTask
{
	class sVehicleMissionParams
	{
	public:
		/* 0x00 = CSyncedEntity, m_TargetEntity, possible "Aggressive NPCs" protection */
		PAD(0x00, 0x2C) float m_fCruiseSpeed;
		float m_fMaxCruiseSpeed;
	};

	INIT_PAD(CTask, 0x90) sVehicleMissionParams m_Params;
};

#pragma pack(pop)
