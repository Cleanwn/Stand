#pragma once

#include "datBitBuffer.hpp"
#include "explosion.hpp"
#include "joaatToString.hpp"

struct CExplosionEvent
{
	CExplosionManager::CExplosionArgs m_explosionArgs{};
	rage::ObjectId m_explodingEntityID = 0;
	rage::ObjectId m_attachEntityID = 0;
	rage::ObjectId m_entExplosionOwnerID = 0;
	rage::ObjectId m_entIgnoreDamageID = 0;
	CNetFXIdentifier m_projectileIdentifier = 0;
	Stand::v3 m_dummyPosition = { 0.0f, 0.0f, 0.0f };
	bool m_bHasProjectile = false;
	bool m_bEntityExploded = false;
	bool m_hasRelatedDummy = false;
	bool m_bUnknown = false;
	bool m_shouldAttach = false;

	CExplosionEvent(rage::datBitBuffer* buffer)
	{
		m_explosionArgs.m_networkIdentifier.m_fxId = buffer->readU16();
		m_explodingEntityID = buffer->readU16(13);
		m_entExplosionOwnerID = buffer->readU16(13);
		m_entIgnoreDamageID = buffer->readU16(13);
		m_explosionArgs.m_explosionTag = (eExplosionTag)buffer->readI32(8);
		m_explosionArgs.m_sizeScale = buffer->readU32(8) / 255.0f;
		m_explosionArgs.m_explosionPosition = buffer->readPosition(22);
		m_hasRelatedDummy = buffer->readBool();
		m_explosionArgs.m_activationDelay = buffer->readU16();
		m_explosionArgs.m_fCamShake = buffer->readSignedFloat(8, 1.0f);
		m_explosionArgs.m_bMakeSound = buffer->readBool();
		m_explosionArgs.m_bNoDamage = buffer->readBool();
		m_explosionArgs.m_bNoFx = buffer->readBool();
		m_explosionArgs.m_bInAir = buffer->readBool();
		m_bUnknown = buffer->readBool();
		m_bEntityExploded = buffer->readBool();
		m_shouldAttach = buffer->readBool();
		m_attachEntityID = buffer->readU16(13);
		buffer->SkipBits(16 + 16 + 16); // m_vDirection
		m_explosionArgs.m_bAttachedToVehicle = buffer->readBool();
		m_explosionArgs.m_bDetonatingOtherPlayersExplosive = buffer->readBool();
		m_explosionArgs.m_weaponHash = buffer->readU32();
		m_explosionArgs.m_interiorLocation.as_uint32 = buffer->readU32();

		SOUP_IF_UNLIKELY (m_hasRelatedDummy)
		{
			m_dummyPosition = buffer->readPosition(31);
		}

		m_bHasProjectile = buffer->readBool();
	}

	[[nodiscard]] bool isSenderResponsible() const noexcept
	{
		return !m_shouldAttach
			&& !m_hasRelatedDummy
			&& !m_bEntityExploded
			&& !m_explosionArgs.m_bDetonatingOtherPlayersExplosive
			;
	}

	[[nodiscard]] bool shouldHaveProjectile() const noexcept
	{
		switch (m_explosionArgs.m_explosionTag)
		{
		case EXP_TAG_GRENADE:
		case EXP_TAG_GRENADELAUNCHER:
		case EXP_TAG_STICKYBOMB:
		case EXP_TAG_MOLOTOV:
		case EXP_TAG_ROCKET:
		case EXP_TAG_TANKSHELL:
		case EXP_TAG_BULLET:
		case EXP_TAG_SMOKEGRENADELAUNCHER:
		case EXP_TAG_SMOKEGRENADE:
		case EXP_TAG_BZGAS:
		case EXP_TAG_FLARE:
		case EXP_TAG_PLANE_ROCKET:
		case EXP_TAG_VEHICLE_BULLET:
		case EXP_TAG_RAILGUN:
		case EXP_TAG_FIREWORK:
		case EXP_TAG_SNOWBALL:
		case EXP_TAG_PROXMINE:
		case EXP_TAG_PIPEBOMB:
		case EXP_TAG_VEHICLEMINE:
		case EXP_TAG_APCSHELL:
		case EXP_TAG_BOMB_CLUSTER:
		case EXP_TAG_BOMB_GAS:
		case EXP_TAG_BOMB_INCENDIARY:
		case EXP_TAG_BOMB_STANDARD:
		case EXP_TAG_TORPEDO:
		case EXP_TAG_TORPEDO_UNDERWATER:
		case EXP_TAG_BOMB_CLUSTER_SECONDARY:
		case EXP_TAG_HUNTER_BARRAGE:
		case EXP_TAG_BOMB_STANDARD_WIDE:
		case EXP_TAG_MORTAR_KINETIC:
		case EXP_TAG_VEHICLEMINE_KINETIC:
		case EXP_TAG_VEHICLEMINE_EMP:
		case EXP_TAG_VEHICLEMINE_SPIKE:
		case EXP_TAG_VEHICLEMINE_SLICK:
		case EXP_TAG_VEHICLEMINE_TAR:
		case EXP_TAG_RAYGUN:
		case EXP_TAG_BURIEDMINE:
		case EXP_TAG_RCTANK_ROCKET:
		case EXP_TAG_BOMB_WATER:
		case EXP_TAG_BOMB_WATER_SECONDARY:
		case EXP_TAG_FLASHGRENADE:
		case EXP_TAG_STUNGRENADE:
		case EXP_TAG_EMPLAUNCHER_EMP:
			return true;
		}

		return false;
	}

	[[nodiscard]] bool canHaveZeroCamShake() const noexcept
	{
		switch (m_explosionArgs.m_explosionTag)
		{
		case EXP_TAG_SNOWBALL:
		case EXP_TAG_BZGAS:
		case EXP_TAG_GAS_CANISTER:
		case EXP_TAG_MOLOTOV:
		case EXP_TAG_BULLET:
		case EXP_TAG_SMOKEGRENADELAUNCHER:
		case EXP_TAG_SMOKEGRENADE:
		case EXP_TAG_FLARE:
		case EXP_TAG_EXPLOSIVEAMMO:
		case EXP_TAG_EXPLOSIVEAMMO_SHOTGUN:
		case EXP_TAG_RAILGUNXM3:
		case EXP_TAG_MINE_CNCSPIKE:
		case EXP_TAG_BZGAS_MK2:
		case EXP_TAG_RAILGUN:
		case EXP_TAG_BIRD_CRAP:
		case EXP_TAG_AIR_DEFENCE: // Potential false negative
		case EXP_TAG_PROGRAMMABLEAR: // Potential false negative
		case EXP_TAG_TRAIN: // Potential false negative
		case EXP_TAG_PROPANE: // Potential false negative
		case EXP_TAG_DIR_FLAME:
		case EXP_TAG_DIR_FLAME_EXPLODE:
		case EXP_TAG_DIR_GAS_CANISTER:
		case EXP_TAG_DIR_WATER_HYDRANT:
		case EXP_TAG_DIR_STEAM:
			return true;
		}

		return false;
	}

	[[nodiscard]] bool shouldHaveDummy() const noexcept
	{
		switch (m_explosionArgs.m_explosionTag)
		{
		case EXP_TAG_DIR_FLAME:
		case EXP_TAG_DIR_GAS_CANISTER:
		case EXP_TAG_DIR_FLAME_EXPLODE:
		case EXP_TAG_DIR_WATER_HYDRANT:
			return true;
		}

		return false;
	}

	[[nodiscard]] bool isProbablyDrone() const noexcept
	{
		return m_explosionArgs.m_fCamShake == 1.0f
			&& m_explosionArgs.m_sizeScale == 0.49803922f
			&& m_explosionArgs.m_explosionTag == EXP_TAG_GRENADE
			;
	}

	[[nodiscard]] const std::string toString() const
	{
		const auto explosion_tag_str = m_explosionArgs.m_explosionTag >= COUNT(Stand::explosion_tag_labels)
			? fmt::to_string(m_explosionArgs.m_explosionTag)
			: fmt::format(fmt::runtime(soup::ObfusString("{} ({})").str()), Stand::explosion_tag_labels[m_explosionArgs.m_explosionTag].getEnglishUtf8(), (uint8_t)m_explosionArgs.m_explosionTag)
			;

#ifdef STAND_DEBUG
		return fmt::format(fmt::runtime(soup::ObfusString(
			"\nm_networkIdentifier: {}\n"
			"m_explodingEntityID: {}\n"
			"m_entExplosionOwnerID: {}\n"
			"m_entIgnoreDamageID: {}\n"
			"m_explosionTag: {}\n"
			"m_sizeScale: {}\n"
			"m_explosionPosition_x: {}\n"
			"m_explosionPosition_y: {}\n"
			"m_explosionPosition_z: {}\n"
			"m_hasRelatedDummy: {}\n"
			"m_activationDelay: {}\n"
			"m_fCamShake: {}\n"
			"m_bMakeSound: {}\n"
			"m_bNoDamage: {}\n"
			"m_bNoFx: {}\n"
			"m_bInAir: {}\n"
			"m_bEntityExploded: {}\n"
			"m_shouldAttach: {}\n"
			"m_attachEntityID: {}\n"
			"m_bAttachedToVehicle: {}\n"
			"m_bDetonatingOtherPlayersExplosive: {}\n"
			"m_weaponHash: {}, {}\n"
			"m_interiorLoc: {}\n"
			"m_bHasProjectile: {}\n"
			"Unknown: {}\n"
		).str()),
			m_explosionArgs.m_networkIdentifier.m_fxId,
			m_explodingEntityID,
			m_entExplosionOwnerID,
			m_entIgnoreDamageID,
			explosion_tag_str,
			m_explosionArgs.m_sizeScale,
			m_explosionArgs.m_explosionPosition.x,
			m_explosionArgs.m_explosionPosition.y,
			m_explosionArgs.m_explosionPosition.z,
			m_hasRelatedDummy,
			m_explosionArgs.m_activationDelay,
			m_explosionArgs.m_fCamShake,
			m_explosionArgs.m_bMakeSound,
			m_explosionArgs.m_bNoDamage,
			m_explosionArgs.m_bNoFx,
			m_explosionArgs.m_bInAir,
			m_bEntityExploded,
			m_shouldAttach,
			m_attachEntityID,
			m_explosionArgs.m_bAttachedToVehicle,
			m_explosionArgs.m_bDetonatingOtherPlayersExplosive,
			m_explosionArgs.m_weaponHash, Stand::joaatToString(m_explosionArgs.m_weaponHash),
			m_explosionArgs.m_interiorLocation.as_uint32,
			m_bHasProjectile,
			m_bUnknown
		);
#else
		return fmt::format(fmt::runtime(soup::ObfusString(" (Type: {})").str()), explosion_tag_str);
#endif
	}
}; 