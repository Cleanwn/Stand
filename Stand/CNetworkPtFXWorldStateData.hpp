#pragma once

#include "datBitBuffer.hpp"

struct CNetworkPtFXWorldStateData
{
	uint32_t m_PtFXHash = 0;
	uint32_t m_PtFXAssetHash = 0;
	Stand::v3 m_FxRot = { 0.0f, 0.0f, 0.0f };
	float m_Scale = 0.0f;
	uint8_t m_InvertAxes = 0;
	bool m_bUseEntity = false;
	uint16_t m_EntityID = 0;
	Stand::v3 m_FxPos = { 0.0f, 0.0f, 0.0f };
	bool m_hasColour = false;
	uint8_t m_r = 0;
	uint8_t m_g = 0;
	uint8_t m_b = 0;
	bool hasBoneIndex = false;
	int32_t m_boneIndex = -1;
	bool hasEvoHashA = false;
	uint32_t m_evoHashA = 0;
	float m_evoValueA = 0.0f;
	bool hasEvoHashB = false;
	uint32_t m_evoHashB = 0;
	float m_evoValueB = 0.0f;
	bool m_bAttachedToWeapon = false;
	bool m_bTerminateOnOwnerLeave = false;
	uint64_t m_ownerPeerID = 0;

	CNetworkPtFXWorldStateData(rage::datBitBuffer* buffer)
	{
		buffer->SkipBits(32); // m_uniqueID
		m_PtFXHash = buffer->readU32();
		m_PtFXAssetHash = buffer->readU32();
		m_FxRot = buffer->readPosition(19);
		m_Scale = (buffer->readU16(10) * 0.00097751711f) * 10.0f;
		m_InvertAxes = buffer->readU8(3);
		m_bUseEntity = buffer->readBool();

		if (m_bUseEntity)
		{
			m_EntityID = buffer->readU16(13);
		}
		else
		{
			m_FxPos = buffer->readPosition(19);
		}

		m_hasColour = buffer->readBool();

		if (m_hasColour)
		{
			m_r = buffer->readU8();
			m_g = buffer->readU8();
			m_b = buffer->readU8();
		}

		hasBoneIndex = buffer->readBool();

		if (hasBoneIndex)
		{
			m_boneIndex = buffer->readU32();
		}

		hasEvoHashA = buffer->readBool();

		if (hasEvoHashA)
		{
			buffer->SkipBits(32 + 10);
		}

		hasEvoHashB = buffer->readBool();

		if (hasEvoHashB)
		{
			buffer->SkipBits(32 + 10);
		}

		m_bAttachedToWeapon = buffer->readBool();
		m_bTerminateOnOwnerLeave = buffer->readBool();

		if (m_bTerminateOnOwnerLeave)
		{
			m_ownerPeerID = buffer->readU64();
		}
	}

	[[nodiscard]] const std::string toString() const
	{
#ifdef STAND_DEV
		return fmt::format(fmt::runtime(soup::ObfusString("PTFX: {}, Dict: {}, Scale: {}, Entity: {}").str()), m_PtFXHash, m_PtFXAssetHash, m_Scale, m_EntityID);
#else
		return "";
#endif
	}
};