#pragma once

#include "atArray.hpp"
#include "ePedVarComp.hpp"
#include "PedProps.hpp"

class CPedVariationData
{
public:
	uint32_t m_aDrawblHash[PV_MAX_COMP];
	PAD(0x30, 0x100);
};
static_assert(sizeof(CPedVariationData) == 0x100);

class CPedSelectionSet
{
};

class CComponentInfo
{
};

class CPVTextureData
{
};

class CPVDrawblData
{
public:
	struct CPVClothComponentData
	{
		/* 0x00 */ bool m_ownsCloth;
		/* 0x01 */ uint8_t m_Padding;
		/* 0x08 */ class characterCloth* m_charCloth;
		/* 0x10 */ int32_t m_dictSlotId;
	};

	/* 0x00 */ uint8_t m_propMask;
	/* 0x01 */ uint8_t m_numAlternatives;
	/* 0x08 */ rage::atArray<CPVTextureData> m_aTexData;
	/* 0x18 */ CPVClothComponentData m_clothData;

	[[nodiscard]] uint32_t GetNumTex() const noexcept
	{
		return m_aTexData.GetCount();
	}
};
static_assert(sizeof(CPVDrawblData) == 0x30);

class CPVComponentData
{
public:
	/* 0x00 */ uint8_t m_numAvailTex;
	/* 0x08 */ rage::atArray<CPVDrawblData> m_aDrawblData3;

	const CPVDrawblData* GetDrawbl(uint32_t i) const noexcept
	{
		//return i < m_aDrawblData3.GetCount() ? &m_aDrawblData3[i] : nullptr;
		return &m_aDrawblData3[i];
	}

	[[nodiscard]] uint32_t GetNumDrawbls() const noexcept
	{
		return m_aDrawblData3.GetCount(); 
	}
};
static_assert(offsetof(CPVComponentData, m_aDrawblData3) == 0x08);

class CPedVariationInfo
{
public:
	bool m_bHasTexVariations;
	bool m_bHasDrawblVariations;
	bool m_bHasLowLODs;
	bool m_bIsSuperLOD;

	uint8_t m_availComp[PV_MAX_COMP];
	/* 0x10 */ rage::atArray<CPVComponentData> m_aComponentData3;
	rage::atArray<CPedSelectionSet> m_aSelectionSets;
	rage::atArray<CComponentInfo> m_compInfos;
	CPedPropInfo m_propInfo;

	rage::atFinalHashString m_dlcName;
	uint8_t m_seqHeadTexIdx;
	uint8_t m_seqHeadDrawblIdx;

	[[nodiscard]] const CPVComponentData* GetCompData(ePedVarComp_t compId) const noexcept
	{
		if (compId >= PV_MAX_COMP)
		{
			return nullptr;
		}

		if (m_availComp[compId] == 0xFF)
		{
			return nullptr;
		}

		if (m_availComp[compId] < m_aComponentData3.size())
		{
			return &m_aComponentData3[m_availComp[compId]];
		}

		return nullptr;
	}

	[[nodiscard]] uint8_t GetMaxNumDrawbls(ePedVarComp_t compType) const noexcept
	{
		if (const CPVComponentData* pCompData = GetCompData(compType))
		{
			return (uint8_t)pCompData->GetNumDrawbls();
		}
		return 0;
	}
};
static_assert(offsetof(CPedVariationInfo, m_aComponentData3) == 0x10);

class CPedVariationInfoCollection
{
public:
	rage::atArray<CPedVariationInfo*> m_infos;

	bool m_bHasLowLODs;
	bool m_bIsStreamed;
	bool m_bIsSuperLOD;

	int32_t m_aAnchorBoneIdxs[NUM_ANCHORS];

	[[nodiscard]] const CPVDrawblData* GetCollectionDrawable(ePedVarComp_t compId, uint32_t drawblIdx) const noexcept
	{
		for (int i = 0; i != m_infos.GetCount(); ++i)
		{
			if (drawblIdx < m_infos[i]->GetMaxNumDrawbls(compId))
			{
				return m_infos[i]->GetCompData(compId)->GetDrawbl(drawblIdx);
			}
			drawblIdx -= m_infos[i]->GetMaxNumDrawbls(compId);
		}
		return nullptr;
	}

	[[nodiscard]] uint32_t GetMaxNumTex(ePedVarComp_t compType, uint32_t drawblIdx) const noexcept
	{
		if (const CPVDrawblData* drawData = GetCollectionDrawable(compType, drawblIdx))
		{
			return drawData->GetNumTex();
		}
		return 0;
	}
};
