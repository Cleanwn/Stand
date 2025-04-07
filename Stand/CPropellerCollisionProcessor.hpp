#pragma once

#include "struct_base.hpp"
#include "gta_fwddecl.hpp"
#include "fwRegdRef.hpp"

struct CPropellerCollisionProcessor
{
	struct CPropellerImpactData
	{
		/* 0x00 */ rage::fwRegdRef<CVehicle> m_pPropOwner;
		/* 0x08 */ rage::fwRegdRef<CEntity> m_pOtherEntity;
		PAD(0x10, 0x60)
	};
	static_assert(sizeof(CPropellerImpactData) == 0x60);

	/* 0x00 */ int m_nNumRotorImpacts;
	PAD(0x04, 0x10) CPropellerImpactData m_aStoredImpacts[20];
};
static_assert(offsetof(CPropellerCollisionProcessor, m_aStoredImpacts) == 0x10);
