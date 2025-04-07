#pragma once

#include "datBitBuffer.hpp"
#include "joaatToString.hpp"

struct CRemoveWeaponEvent
{
	uint16_t m_pedID = 0;
	uint32_t m_weaponHash = 0;

	CRemoveWeaponEvent(rage::datBitBuffer* buffer)
	{
		m_pedID = buffer->readU16(13);
		m_weaponHash = buffer->readU32();
	}

	[[nodiscard]] const std::string toString() const
	{
		return fmt::format(fmt::runtime(soup::ObfusString(" (Entity: {}, Weapon: {})").str()), m_pedID, Stand::joaatToString(m_weaponHash));
	}
};