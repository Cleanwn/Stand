#pragma once

#include "gta_fwddecl.hpp"

#include "atFixedArray.hpp"
#include "fwRefAwareBase.hpp"

enum ePedTaskTrees
{
	PED_TASK_TREE_PRIMARY = 0,
	PED_TASK_TREE_SECONDARY,
	PED_TASK_TREE_MOVEMENT,
	PED_TASK_TREE_MOTION,
	PED_TASK_TREE_MAX
};

class CPedTaskManager
{
public:
	virtual ~CPedTaskManager() = default;

	rage::atFixedArray<rage::aiTaskTree*, 4> trees; // access element 0, can assume it's not nullptr, not sure if and when the others are used
};

class CTaskInfo
{
public:
	virtual ~CTaskInfo() = default;

	/* 0x08 */ uint32_t task_id;
	/* 0x0C */ uint32_t bitfield;
	/* 0x10 */ CTaskInfo* next;

	[[nodiscard]] uint32_t getPriority() const noexcept
	{
		return (bitfield >> 1) & 7u;
	}
};
static_assert(sizeof(CTaskInfo) == 0x18);

class CFiringPatternInfo : public rage::fwRefAwareBase
{
public:
	virtual ~CFiringPatternInfo() = default;
};

class CRelationshipGroup : public rage::fwRefAwareBase
{
public:
	virtual ~CRelationshipGroup() = default;

	/* 0x10 */ uint32_t m_eType;
	/* 0x14 */ rage::atHashString m_name;
};
static_assert(offsetof(CRelationshipGroup, m_name) == 0x14);

class CPedIntelligence
{
public:
	PAD(0, 0x358) CPed* ped;
	PAD(0x358 + 8, 0x368) CPedTaskManager task_manager;
	PAD(0x368 + sizeof(CPedTaskManager), 0xB08) CTaskInfo* task_info;
	PAD(0xB08 + 8, 0xBE0) CFiringPatternInfo* firing_pattern_info;
	PAD(0xBE0 + 8, 0xC20) CRelationshipGroup* m_pRelationshipGroupDefault;
	/* 0xC28 */ CRelationshipGroup* m_pRelationshipGroup;

	[[nodiscard]] CRelationshipGroup* GetRelationshipGroup() const noexcept
	{
		if (m_pRelationshipGroup)
		{
			return m_pRelationshipGroup;
		}
		if (m_pRelationshipGroupDefault)
		{
			return m_pRelationshipGroupDefault;
		}
		return nullptr;
	}

	[[nodiscard]] CTask* FindTaskActiveByType(const int iType) const noexcept;
};
static_assert(sizeof(CPedIntelligence) == 0xC28 + 8);
