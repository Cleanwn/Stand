#pragma once

#include "datBitBuffer.hpp"
#include "eDamageFlags.hpp"
#include "joaatToString.hpp"
#include "vector.hpp"
#include "weapons.hpp"

namespace Stand
{
	class CWeaponDamageEvent
	{
	public:
		uint8_t m_damageType = 0;
		uint32_t m_weaponType = 0;
		bool m_bOverride = false;
		bool m_hitEntityWeapon = false;
		bool m_hitWeaponAmmoAttachment = false;
		bool m_silenced = false;
		uint32_t m_damageFlags = 0;
		bool bMeleeDamage = false;
		uint32_t m_actionResultId = 0;
		uint16_t m_meleeId = 0;
		uint32_t m_meleeResultId = 0;
		uint32_t m_weaponDamage = 0;
		bool bIsAggregated = false;
		uint8_t m_weaponDamageAggregationCount = 0;
		bool m_bVictimPlayer = false; // Unreliable as of late.
		v3 m_hitPosition = { 0.0f, 0.0f, 0.0f };
		uint32_t m_damageTime = 0;
		bool m_willKillPlayer = false; // Unreliable as of late.
		uint16_t m_hitObjectId = 0; // Only represents melee hits.
		bool useLargeDistance = false;
		uint16_t m_playerDistance = 0;
		uint16_t m_parentID = 0;
		bool hasVehicleData = false;
		uint8_t m_tyreIndex = 0;
		uint8_t m_suspensionIndex = 0;
		uint8_t m_component = 0;
		bool m_firstBullet = false;
		bool m_hasHitDirection = false;
		v3 m_hitDirection = { 0.0f, 0.0f, 0.0f };

		CWeaponDamageEvent(rage::datBitBuffer* buffer)
		{
			m_damageType = buffer->readU8(2);
			m_weaponType = buffer->readU32();
			m_bOverride = buffer->readBool();
			m_hitEntityWeapon = buffer->readBool();
			m_hitWeaponAmmoAttachment = buffer->readBool();
			m_silenced = buffer->readBool();
			m_damageFlags = buffer->readU32(25); // 24 -> 25 in 3258
			bMeleeDamage = buffer->readBool();

			if (bMeleeDamage)
			{
				m_actionResultId = buffer->readU32();
				m_meleeId = buffer->readU16();
				m_meleeResultId = buffer->readU32();
			}

			if (m_bOverride)
			{
				m_weaponDamage = buffer->readU32(17);
			}

			bIsAggregated = buffer->readBool();
			if (bIsAggregated)
			{
				m_weaponDamageAggregationCount = buffer->readU8(5); // 4 -> 5 in 3258
			}

			m_bVictimPlayer = buffer->readBool();
			if (m_bVictimPlayer)
			{
				m_hitPosition.x = buffer->readSignedFloat(16, 90.0f);
				m_hitPosition.y = buffer->readSignedFloat(16, 90.0f);
				m_hitPosition.z = buffer->readSignedFloat(16, 90.0f);
			}

			if (m_damageType == 3) // 3 == DAMAGE_PLAYER
			{
				m_damageTime = buffer->readU32();
				m_willKillPlayer = buffer->readBool();

				if (bMeleeDamage)
				{
					m_hitObjectId = buffer->readU16(13);
				}

				useLargeDistance = buffer->readBool();
				m_playerDistance = useLargeDistance ? buffer->readU16() : buffer->readU16(11);
			}
			else
			{
				m_parentID = buffer->readU16(13);
			}

			if (m_damageType < 2u) // DAMAGE_OBJECT == 0, DAMAGE_VEHICLE == 1
			{
				m_hitPosition.x = buffer->readSignedFloat(16, 90.0f);
				m_hitPosition.y = buffer->readSignedFloat(16, 90.0f);
				m_hitPosition.z = buffer->readSignedFloat(16, 90.0f);

				if (m_damageType == 1) // DAMAGE_VEHICLE
				{
					hasVehicleData = buffer->readBool();
					if (hasVehicleData)
					{
						m_tyreIndex = buffer->readU8(4);
						m_suspensionIndex = buffer->readU8(4);
					}
				}
			}
			else
			{
				m_component = buffer->readU8(5);
			}

			m_firstBullet = buffer->readBool();
			m_hasHitDirection = buffer->readBool();
			if (m_hasHitDirection)
			{
				m_hitDirection.x = buffer->readSignedFloat(16, 6.2831855f);
				m_hitDirection.y = buffer->readSignedFloat(16, 6.2831855f);
				m_hitDirection.z = buffer->readSignedFloat(16, 6.2831855f);
			}
		}

		[[nodiscard]] bool hasFlag(Stand::eDamageFlags flag) const noexcept
		{
			return m_damageFlags & flag;
		}

		[[nodiscard]] bool isDamageExcessive() const noexcept
		{
			return m_weaponDamage > 9999 && m_weaponType != ATSTRINGHASH("WEAPON_EXPLOSION");
		}

		[[nodiscard]] bool isRemoteKill(bool is_beast) const noexcept
		{
			if (bMeleeDamage != hasFlag(eDamageFlags::MeleeDamage))
			{
				return true;
			}

			if ((m_weaponType == ATSTRINGHASH("WEAPON_STUNGUN") || m_weaponType == ATSTRINGHASH("WEAPON_STUNGUN_MP"))
				&& hasFlag(eDamageFlags::IgnoreRemoteDistCheck)
				)
			{
				return true;
			}

			if (m_weaponType == ATSTRINGHASH("WEAPON_EXPLOSION") && m_silenced)
			{
				return true;
			}

			if (!is_beast
				&& m_damageType == 3
				//&& m_bVictimPlayer
				//&& m_willKillPlayer
				&& isDamageExcessive()
				)
			{
				return true;
			}

			if (m_damageType == 3) // m_bVictimPlayer doesn't always match this.
			{
				if (const Weapon* weapon = Weapon::find(m_weaponType); m_weaponType == ATSTRINGHASH("WEAPON_UNARMED") || (weapon && weapon->category == Weapon::MELEE))
				{
					if (m_hitObjectId == 0 || useLargeDistance)
					{
						return true;
					}
				}
			}

			return false;
		}

		// Warrants either removal or improvement if anyone else decides to implement our magic bullets. So far, we're alone, so this is okay.
		[[nodiscard]] bool isMagicBullet() const noexcept
		{
			return m_bOverride
				&& !m_tyreIndex
				&& !m_suspensionIndex
				&& (m_damageFlags == (eDamageFlags::IsAccurate | eDamageFlags::AllowHeadShot | eDamageFlags::IgnorePedFlags))
				&& !m_actionResultId
				&& !m_meleeId
				&& !m_hitEntityWeapon
				&& !m_hitWeaponAmmoAttachment
				&& !m_silenced
				&& !m_firstBullet
				&& m_hitDirection.isNull()
				;
		}

		[[nodiscard]] const std::string toString() const
		{
#ifdef STAND_DEBUG
			return fmt::format(fmt::runtime(soup::ObfusString(
				" ("
				"m_damageType: {}, "
				"m_weaponType: {}, "
				"m_bOverride: {}, "
				"m_hitEntityWeapon: {}, "
				"m_hitWeaponAmmoAttachment: {}, "
				"m_silenced: {}, "
				"m_damageFlags: {}, "
				"bMeleeDamage: {}, "
				"m_actionResultId: {}, "
				"m_meleeId: {}, "
				"m_meleeResultId: {}, "
				"m_weaponDamage: {}, "
				"m_bIsAggregated: {}, "
				"m_weaponDamageAggregationCount: {}, "
				"m_bVictimPlayer: {}, "
				"m_hitPosition_x: {}, "
				"m_hitPosition_y: {}, "
				"m_hitPosition_z: {}, "
				"m_damageTime: {}, "
				"m_willKillPlayer: {}, "
				"m_hitObjectId: {}, "
				"m_playerDistance: {}, "
				"m_parentID: {}, "
				"m_tyreIndex: {}, "
				"m_suspensionIndex: {}, "
				"m_component: {}, "
				"m_firstBullet: {}, "
				"hasVehicleData: {}, "
				"useLargeDistance: {}"
				")"
			).str()),
				m_damageType,
				Stand::joaatToString(m_weaponType),
				m_bOverride,
				m_hitEntityWeapon,
				m_hitWeaponAmmoAttachment,
				m_silenced,
				m_damageFlags,
				bMeleeDamage,
				m_actionResultId,
				m_meleeId,
				m_meleeResultId,
				m_weaponDamage,
				bIsAggregated,
				m_weaponDamageAggregationCount,
				m_bVictimPlayer,
				m_hitPosition.x,
				m_hitPosition.y,
				m_hitPosition.z,
				m_damageTime,
				m_willKillPlayer,
				m_hitObjectId,
				m_playerDistance,
				m_parentID,
				m_tyreIndex,
				m_suspensionIndex,
				m_component,
				m_firstBullet,
				hasVehicleData,
				useLargeDistance
			);
#else
			return fmt::format(fmt::runtime(soup::ObfusString(" (Type: {}, Damage: {}, Flags: {}, {}, {})").str()), Stand::joaatToString(m_weaponType), m_weaponDamage, m_damageFlags, m_bVictimPlayer, m_damageType);
#endif
		}
	};
}