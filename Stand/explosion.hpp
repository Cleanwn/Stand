#pragma once

#include "struct_base.hpp"

#include "CNetFXIdentifier.hpp"
#include "eExplosionTag.hpp"
#include "fwInteriorLocation.hpp"

#define DEFAULT_EXPLOSION_SCALE 1.0f

struct CExplosionManager
{
	class CExplosionArgs
	{
	public:
		CExplosionArgs() = default;

		CExplosionArgs(eExplosionTag explosionTag, const Stand::v3& explosionPosition)
			: m_explosionTag(explosionTag), m_explosionPosition(explosionPosition)
		{
		}

		CExplosionArgs(CExplosionArgs& args) :
			m_explosionTag(args.m_explosionTag),
			m_explosionPosition(args.m_explosionPosition),
			m_pExplodingEntity(args.m_pExplodingEntity),
			m_pEntExplosionOwner(args.m_pEntExplosionOwner),
			m_pEntIgnoreDamage(args.m_pEntIgnoreDamage),
			m_activationDelay(args.m_activationDelay),
			m_sizeScale(args.m_sizeScale),
			m_camShakeNameHash(args.m_camShakeNameHash),
			m_fCamShake(args.m_fCamShake),
			m_fCamShakeRollOffScaling(args.m_fCamShakeRollOffScaling),
			m_bMakeSound(args.m_bMakeSound),
			m_bNoFx(args.m_bNoFx),
			m_bInAir(args.m_bInAir),
			m_vDirection(args.m_vDirection),
			m_pAttachEntity(args.m_pAttachEntity),
			m_attachBoneTag(args.m_attachBoneTag),
			m_originalExplosionTag(args.m_originalExplosionTag),
			m_vfxTagHash(args.m_vfxTagHash),
			m_weaponHash(args.m_weaponHash),
			m_pRelatedDummyForNetwork(args.m_pRelatedDummyForNetwork),
			m_networkIdentifier(args.m_networkIdentifier),
			m_bIsLocalOnly(args.m_bIsLocalOnly),
			m_bNoDamage(args.m_bNoDamage),
			m_bAttachedToVehicle(args.m_bAttachedToVehicle),
			m_bDetonatingOtherPlayersExplosive(args.m_bDetonatingOtherPlayersExplosive),
			m_bDisableDamagingOwner(args.m_bDisableDamagingOwner),
			m_interiorLocation(args.m_interiorLocation)
		{
		}

		/* 0x00 */ eExplosionTag m_explosionTag;
		PAD(0x04, 0x10) rage::Vector3 m_explosionPosition;
		/* 0x20 */ CEntity* m_pExplodingEntity = nullptr;
		/* 0x28 */ CEntity* m_pEntExplosionOwner = nullptr;
		/* 0x30 */ CEntity* m_pEntIgnoreDamage = nullptr;
		/* 0x38 */ uint32_t m_activationDelay = 0;
		/* 0x3C */ float m_sizeScale = DEFAULT_EXPLOSION_SCALE;
		/* 0x40 */ uint32_t m_camShakeNameHash = 0;
		/* 0x44 */ float m_fCamShake = -1.0f;
		/* 0x48 */ float m_fCamShakeRollOffScaling = -1.0f;
		/* 0x4C */ bool	m_bMakeSound = true;
		/* 0x4D */ bool m_bNoFx = false;
		/* 0x4E */ bool	m_bInAir = false;
		/* 0x50 */ rage::Vector3 m_vDirection{ 0.0f, 0.0f, 1.0f };
		/* 0x60 */ CEntity* m_pAttachEntity = nullptr;
		/* 0x68 */ int32_t m_attachBoneTag = 0;
		/* 0x6C */ eExplosionTag m_originalExplosionTag = EXP_TAG_DONTCARE;
		/* 0x70 */ uint32_t m_vfxTagHash = 0;
		/* 0x74 */ uint32_t m_weaponHash = ATSTRINGHASH("WEAPON_EXPLOSION");
		/* 0x78 */ rage::fwInteriorLocation m_interiorLocation;
		/* 0x80 */ class CDummyObject* m_pRelatedDummyForNetwork = nullptr;
		/* 0x88 */ CNetFXIdentifier	m_networkIdentifier;
		/* 0x8C */ bool	m_bIsLocalOnly = false;
		/* 0x8D */ bool	m_bNoDamage = false;
		/* 0x8E */ bool	m_bAttachedToVehicle = false;
		/* 0x8F */ bool m_bDetonatingOtherPlayersExplosive = false;
		/* 0x90 */ bool m_bDisableDamagingOwner = false;
	};
	static_assert(offsetof(CExplosionArgs, m_bDisableDamagingOwner) == 0x90);
};
