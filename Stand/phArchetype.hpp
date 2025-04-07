#pragma once

#include "struct_base.hpp"
#include "pgBase.hpp"

namespace rage
{
	struct phArchetypeBase : public pgBase
	{
	};

	struct phArchetype : public phArchetypeBase
	{
		INIT_PAD(phArchetypeBase, 0x18) const char* m_Filename; // Model name, i.e: biff, emperor2, benson2
		PAD(0x18 + 8, 0x28) uint32_t m_TypeFlags;
		/* 0x2C */ uint32_t m_IncludeFlags;
		/* 0x30 */ uint32_t m_PropertyFlags;
		PAD(0x30 + 4, 0x38);
	};
	static_assert(sizeof(phArchetype) == 0x38);

	struct phArchetypePhys : public phArchetype
	{
		/* 0x38 */ uint8_t m_ArchetypePhysPad[8];
		/* 0x40 */ float m_Mass;
		/* 0x44 */ float m_InvMass;
		/* 0x48 */ float m_GravityFactor;
		/* 0x4C */ float m_MaxSpeed;
		/* 0x50 */ float m_MaxAngSpeed;
		/* 0x54 */ float m_BuoyancyFactor;
	};
	static_assert(sizeof(phArchetypePhys) == 0x54 + 4);

	struct phArchetypeDamp : public phArchetypePhys
	{
	};
}
