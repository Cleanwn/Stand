#pragma once

#include "datBitBuffer.hpp"
#include "vector.hpp"

struct CStartProjectileEvent
{
	uint16_t m_projectileOwnerID = 0;
	uint32_t m_projectileHash = 0;
	uint32_t m_weaponFiredFromHash = 0;
	Stand::v3 m_initialPosition = { 0.0f, 0.0f, 0.0f };
	uint16_t m_targetEntityID = 0;
	Stand::v3 m_fireDirection = { 0.0f, 0.0f, 0.0f };
	uint8_t m_effectGroup = 0;
	uint8_t m_tintIndex = 0;
	bool m_bAllowDamping = false;
	bool m_bVehicleRelativeOffset = false;
	bool m_bCommandFireSingleBullet = false;
	uint8_t m_vehicleFakeSequenceId = 0;
	Stand::v3 m_vVehicleRelativeOffset = { 0.0f, 0.0f, 0.0f };
	bool m_coordinatedWithTask = false;
	float m_fLaunchSpeedOverride = 0.0f;
	uint32_t m_coordinateWithTaskSequence;
	bool m_bWasLockedOnWhenFired = false;
	uint16_t m_ignoreDamageEntId = 0;
	uint16_t m_noCollisionEntId = 0;
	bool m_bSyncOrientation = false;
	uint16_t m_netIdentifier_m_fxId = 0;

	CStartProjectileEvent(rage::datBitBuffer* buffer)
	{
		m_projectileOwnerID = buffer->readU16(13);
		m_projectileHash = buffer->readU32();
		m_weaponFiredFromHash = buffer->readU32();
		m_initialPosition = buffer->readVector(32, 16000.0f);
		m_targetEntityID = buffer->readU16(13);
		m_fireDirection = buffer->readVector(16, 1.1f);
		m_effectGroup = buffer->readU8(5);
		m_tintIndex = buffer->readU8();
		m_bAllowDamping = buffer->readBool();
		m_bVehicleRelativeOffset = buffer->readBool();
		m_bCommandFireSingleBullet = buffer->readBool();

		if (buffer->readBool()) // m_vehicleFakeSequenceId != 0xFF
		{
			m_vehicleFakeSequenceId = buffer->readU8(7);
		}

		if (m_bVehicleRelativeOffset)
		{
			m_vVehicleRelativeOffset = buffer->readVector(16, 400.0f);
		}

		m_coordinatedWithTask = buffer->readBool();

		if (buffer->readBool()) // m_fLaunchSpeedOverride >= 0.0
		{
			m_fLaunchSpeedOverride = (buffer->readU32(18) * 0.0000038147118f) * 8000.0f;
		}

		if (m_coordinatedWithTask)
		{
			m_coordinateWithTaskSequence = buffer->readU32();
		}

		m_bWasLockedOnWhenFired = buffer->readBool();
		m_ignoreDamageEntId = buffer->readU16(13);
		m_noCollisionEntId = buffer->readU16(13);
		m_bSyncOrientation = buffer->readBool();

		if (m_bSyncOrientation)
		{
			buffer->SkipBits(9 + 9 + 9); // m_projectileWeaponMatrix
		}

		m_netIdentifier_m_fxId = buffer->readU16();
	}

	[[nodiscard]] const std::string toString() const
	{
#ifdef STAND_DEBUG
		return fmt::format(" ("
			"m_projectileOwnerID: {}, "
			"m_projectileHash: {}, "
			"m_weaponFiredFromHash: {}, "
			"m_targetEntityID: {}, "
			"m_effectGroup: {}, "
			"m_tintIndex: {}, "
			"m_bAllowDamping: {}, "
			"m_bVehicleRelativeOffset: {}, "
			"m_bCommandFireSingleBullet: {}, "
			"m_vehicleFakeSequenceId: {}, "
			"m_coordinatedWithTask: {}, "
			"m_fLaunchSpeedOverride: {}, "
			"m_coordinateWithTaskSequence: {}, "
			"m_bWasLockedOnWhenFired: {}, "
			"m_ignoreDamageEntId: {}, "
			"m_noCollisionEntId: {}, "
			"m_bSyncOrientation: {}, "
			"m_netIdentifier_m_fxId: {})",
			m_projectileOwnerID,
			m_projectileHash,
			m_weaponFiredFromHash,
			m_targetEntityID,
			m_effectGroup,
			m_tintIndex,
			m_bAllowDamping,
			m_bVehicleRelativeOffset,
			m_bCommandFireSingleBullet,
			m_vehicleFakeSequenceId,
			m_coordinatedWithTask,
			m_fLaunchSpeedOverride,
			m_coordinateWithTaskSequence,
			m_bWasLockedOnWhenFired,
			m_ignoreDamageEntId,
			m_noCollisionEntId,
			m_bSyncOrientation,
			m_netIdentifier_m_fxId
		);
#else
		return "";
#endif
	}
};