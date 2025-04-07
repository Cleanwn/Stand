#pragma once

#include "CBaseModelInfo.hpp"

#include "atString.hpp"
#include "PedVariationDS.hpp"

class CPedModelInfo : public CBaseModelInfo
{
	INIT_PAD(CBaseModelInfo, 0x1C8) rage::atString m_StreamFolder;
	/* 0x1D8 */ rage::atString m_propStreamFolder;
	PAD(0x1D8 + 0x10, 0x240) CPedVariationInfoCollection* m_pPedVarInfoCollection; // May be nullptr if model is not needed
	PAD(0x240 + 8, 0x280) int m_pedAbilityType;
	/* 0x284 */ bool m_bIs_IG_Ped;
	/* 0x285 */ uint8_t m_numAvailableLODs;
	/* 0x286 */ uint8_t m_CountedAsScenarioPed : 1;
	uint8_t m_isStreamedGraphics : 1;
	uint8_t	m_AmbulanceShouldRespondTo : 1;
	uint8_t	m_ThermalBehaviour : 2;
	uint8_t	m_SuperlodType : 3;
	int8_t m_isRequestedAsDriver;
	uint8_t	m_CanRideBikeWithNoHelmet : 1;
	uint8_t	m_CanSpawnInCar : 1;
	uint8_t	m_bIsHeadBlendPed : 1;
	uint8_t m_bOnlyBulkyItemVariations : 1;
	uint8_t	m_pad : 4;

	[[nodiscard]] const CPedVariationInfoCollection* GetVarInfo() const noexcept
	{
		return m_pPedVarInfoCollection;
	}
};
static_assert(sizeof(CPedModelInfo) == 0x286 + 10);
static_assert(offsetof(CPedModelInfo, m_StreamFolder) == 0x1C8);
static_assert(offsetof(CPedModelInfo, m_propStreamFolder) == 0x1D8);
