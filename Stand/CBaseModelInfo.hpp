#pragma once

#include "struct_base.hpp"

#include "gta_fwddecl.hpp"

#include "datBase.hpp"
#include "gta_extensible.hpp"

namespace rage
{
	class fwDynamicArchetypeComponent
	{
	public:
		/* 0x00 */ rage::Vec4V m_AIAvoidancePoint;
		/* 0x10 */ rage::atHashString m_expressionHash;
		/* 0x18 */ phArchetypeDamp* m_pPhysicsArch;
		int16_t m_clipDictionaryIndex;
		int16_t m_blendShapeFileIndex;
		int16_t m_expressionDictionaryIndex;
		uint16_t m_physicsArchRefs;
		int8_t m_poseMatcherFileIndex;
		int8_t m_poseMatcherProneFileIndex;
		int16_t m_creatureMetadataFileIndex : 14;
		uint16_t m_bAutoStartAnim : 1;
		uint16_t m_bHasPhysicsInDrawable : 1;
	};
	static_assert(offsetof(fwDynamicArchetypeComponent, m_pPhysicsArch) == 0x18);

#pragma pack(push, 1)
	class fwArchetype : public datBase
	{
	public:
		enum DrawableType
		{
			DT_UNINITIALIZED,
			DT_FRAGMENT,
			DT_DRAWABLE,
			DT_DRAWABLEDICTIONARY,
			DT_ASSETLESS,
			DT_COUNT
		};

		virtual ~fwArchetype() = default;

		/* 0x08 */ fwExtensionList extension_list;
		/* 0x18 */ hash_t hash;
		PAD(0x18 + 4, 0x50) uint32_t flags; // 1 = MODEL_HAS_LOADED, 0x10 = spawnable object, 0x80000000 = streamed (non-permanent) archetype
										    // oppressor         -> 8000 100X
										    // dt1_01_build      -> 8000 010X
										    // prop_gold_cont_01 -> 0000 111X
										    // 02gate3_l         -> 0000 981X
										    // 02gate3_l a.s.    -> 0000 989X
											// prop_roofvent_06a -> 3800 1113
										    // X = 0 before spawn, 1 after spawn (a.s.)
		PAD(0x54, 0x58) fwDynamicArchetypeComponent* m_DynamicComponent;
		/* 0x60 */ uint8_t m_DrawableType;
		PAD(0x60 + 1, 0x6A) uint16_t m_assignedStreamingSlot;
		PAD(0x6A + 2, 0x70);

		[[nodiscard]] uint32_t getAttribute() const noexcept
		{
			return (flags >> 26) & 0b11111;
		}

		[[nodiscard]] bool GetHasBoundInDrawable() const noexcept
		{
			return m_DrawableType == DT_DRAWABLE
				&& m_DynamicComponent
				&& m_DynamicComponent->m_bHasPhysicsInDrawable
				;
		}
	};
	static_assert(sizeof(fwArchetype) == 0x70);
}

enum ModelInfoType : uint8_t
{
	MI_TYPE_NONE,
	MI_TYPE_BASE,
	MI_TYPE_MLO,
	MI_TYPE_TIME,
	MI_TYPE_WEAPON,
	MI_TYPE_VEHICLE,
	MI_TYPE_PED,
	MI_TYPE_COMPOSITE,
};

class CBaseModelInfo : public rage::fwArchetype
{
public:
	virtual ~CBaseModelInfo() = default;
	virtual void Init() = 0; // 0x38

	/* 0x70 */ CCustomShaderEffectBaseType* custom_shader_effect;
	PAD(0x70 + 8, 0x9D) uint8_t m_type : 5;
	PAD(0x9D + 1, 0xB0);

	[[nodiscard]] uint8_t GetModelType() const noexcept
	{
		return m_type;
	}
};
static_assert(sizeof(CBaseModelInfo) == 0xB0);
#pragma pack(pop)
