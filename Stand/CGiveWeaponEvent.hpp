#pragma once

#include "datBitBuffer.hpp"

struct CGiveWeaponEvent
{
	uint16_t m_pedID;
	uint32_t m_weaponHash;
	int16_t m_ammo;
	bool m_bAsPickup;

	CGiveWeaponEvent(rage::datBitBuffer* buffer)
	{
		m_pedID = buffer->readU16(13);
		m_weaponHash = buffer->readU32();
		m_ammo = buffer->readI16();
		m_bAsPickup = buffer->readBool();
	}

	[[nodiscard]] const std::string toString() const
	{
		return fmt::format(fmt::runtime(soup::ObfusString(" ({}, {}, {}, {})").str()), m_pedID, Stand::joaatToString(m_weaponHash), m_ammo, m_bAsPickup);
	}
};