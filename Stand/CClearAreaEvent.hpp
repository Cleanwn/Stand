#pragma once

#include "datBitBuffer.hpp"
#include "vector.hpp"

struct CClearAreaEvent
{
	Stand::v3 m_areaCentre = { 0.0f, 0.0f, 0.0f };
	float m_areaRadius = 0.0f;
	uint16_t m_flags = 0;

	CClearAreaEvent(rage::datBitBuffer* buffer)
	{
		m_areaCentre = buffer->readPosition(19);
		m_areaRadius = (buffer->readU16(15) * 0.000030518509f) * 16384.0f;
		m_flags = buffer->readU16(11);
	}

	[[nodiscard]] const std::string toString() const
	{
		return fmt::format(fmt::runtime(soup::ObfusString(" ({}, {})").str()), m_areaRadius, m_flags);
	}
};