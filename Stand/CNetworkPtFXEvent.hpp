#pragma once

#include "datBitBuffer.hpp"
#include "vector.hpp"

struct CNetworkPtFXEvent
{
	hash_t m_PtFXHash = 0;
	hash_t m_PtFXAssetHash = 0;
	Stand::v3 m_FxPos = { 0.0f, 0.0f, 0.0f };
	Stand::v3 m_FxRot = { 0.0f, 0.0f, 0.0f };
	float m_Scale = 0.0f;
	uint8_t m_InvertAxes = 0;
	bool m_bUseEntity = false;
	uint16_t m_EntityID = 0;

	CNetworkPtFXEvent(rage::datBitBuffer* buffer)
	{
		m_PtFXHash = buffer->readU32();
		m_PtFXAssetHash = buffer->readU32();
		m_FxPos = buffer->readPosition(19);
		m_FxRot = buffer->readPosition(19);
		m_Scale = (buffer->readU16(10) * 0.00097751711f) * 10.0f;
		m_InvertAxes = buffer->readU8(3);
		m_bUseEntity = buffer->readBool();

		if (m_bUseEntity)
		{
			m_EntityID = buffer->readU16(13);
		}
	}

	[[nodiscard]] const std::string toString() const
	{
		return fmt::format(fmt::runtime(soup::ObfusString(" (Hash: {}, Position: ({}, {}, {}), Entity: {})").str()),
			m_PtFXHash,
			m_FxPos.x,
			m_FxPos.y,
			m_FxPos.z,
			m_EntityID
		);
	}
};