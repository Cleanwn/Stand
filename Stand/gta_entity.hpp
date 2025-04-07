#pragma once

#include <soup/Matrix.hpp>

#include "struct_base.hpp"

#include "CBaseModelInfo.hpp"
#include "datRef.hpp"
#include "fragCacheEntry.hpp"
#include "gta_extensible.hpp"
#include "gta_draw_handler.hpp"
#include "pgBase.hpp"

namespace rage
{
	class phInst : public pgBase
	{
	public:
		enum
		{
			FLAG_USERDATA_ENTITY = BIT(9),
			FLAG_USER_0 = BIT(10)
		};

		/* 0x10 */ phArchetype* archetype;
		/* 0x18 */ uint16_t m_LevelIndex;
		/* 0x1A */ uint16_t m_Flags;
		PAD(0x1C, 0x20) rage::Mat34V m_Matrix;

		[[nodiscard]] bool IsInLevel() const noexcept
		{
			return m_LevelIndex != -1;
		}

		[[nodiscard]] CPhysical* getPhysical() const noexcept
		{
			if (m_Flags & FLAG_USERDATA_ENTITY)
			{
				return (CPhysical*)(uint64_t(m_Matrix.m_col3.m128_u32[3]) | (uint64_t(m_Matrix.m_col2.m128_u32[3]) << 32));
			}

			return nullptr;
		}
	};
	static_assert(sizeof(phInst) == 0x20 + sizeof(soup::Matrix));
	static_assert(offsetof(phInst, m_Matrix) == 0x20);

	struct phInstBreakable : public phInst
	{
	};

	struct fragInst : public phInstBreakable
	{
		INIT_PAD(phInstBreakable, 0x68) rage::datRef<rage::fragCacheEntry> m_CacheEntry;
	};

	struct phfwInst : public phInst
	{
	};

	struct phfwFragInst : public fragInst
	{
	};
}

class phInstGta : public rage::phfwInst
{
};

class fragInstGta : public rage::phfwFragInst
{
};

class CPortalTrackerBase
{
public:
	virtual ~CPortalTrackerBase() = default;

	/* 0x08 */ rage::fwEntity* entity;
};
static_assert(sizeof(CPortalTrackerBase) == 0x10);

class CPortalTracker : public CPortalTrackerBase
{
};

namespace rage
{
	class fwAttachmentEntityExtension
	{
	public:
		/* 0x00 */ fwEntity* m_pAttachParent;
		/* 0x08 */ fwEntity* m_pAttachChild;
		/* 0x10 */ fwEntity* m_pAttachSibling;
		/* 0x18 */ fwEntity* m_pThisEntity;
	};
	static_assert(sizeof(fwAttachmentEntityExtension) == 0x18 + 8);
	
	class fwDynamicEntityComponent
	{
	public:
		enum : uint8_t // eNOCollisionFlags
		{
			NO_COLLISION_HIT_SHOULD_FIND_IMPACTS = BIT(0),
			NO_COLLISION_NETWORK_OBJECTS = BIT(1),
			NO_COLLISION_PERMENANT = BIT(2),
			NO_COLLISION_RESET_WHEN_NO_BBOX = BIT(3),
			NO_COLLISION_RESET_WHEN_NO_IMPACTS = BIT(4),
			NO_COLLISION_NEEDS_RESET = BIT(5)
		};

		rage::Vector3 m_vAnimatedVelocity;
		rage::Vector3 m_vecPrevPos;

		class fwEntityDesc* m_pEntityDesc;
		class crSkeleton* m_pSkeleton;
		class crCreature* m_pCreature;
		class grbTargetManager* m_pTargetManager;
		class fwAnimDirector* m_pAnimDirector;
		/* 0x48 */ fwAttachmentEntityExtension* m_pAttachmentEntityExtension;
		/* 0x50 */ phCollider* m_pCollider;
		/* 0x58 */ fwEntity* m_pNoCollisionEntity;
		/* 0x60 */ float m_fAnimatedAngularVelocity;
		/* 0x64 */ float m_fForceAddToBoundRadius;
		/* 0x68 */ uint8_t m_nNoCollisionFlags;
	};
	static_assert(offsetof(fwDynamicEntityComponent, m_nNoCollisionFlags) == 0x68);
	static_assert(sizeof(fwDynamicEntityComponent) == 0x68 + 1 + 7);

#pragma pack(push, 1)
	class fwEntity : public fwExtensibleBase
	{
	public:
		enum Flags
		{
			IS_FIXED = 0x20000
		};

		// there is quite some structs that probably depend on the vftable size of fwEntity...
		// unfortunately we can't exactly static_assert this, and I don't know where it would cause problems.
		// so let's just not.
#if false
		virtual void _0x38() = 0;
		virtual void _0x40() = 0;
		virtual void _0x48() = 0;
		virtual void _0x50() = 0;
		virtual void _0x58() = 0;
		virtual void _0x60() = 0;
		virtual void _0x68() = 0;
		virtual void _0x70() = 0;
		virtual fragInst* GetFragInst() const = 0;
#endif

		/* 0x20 */ class CBaseModelInfo* archetype; // same in 323
		/* 0x28 */ uint8_t type;
		/* 0x29 */ uint8_t protected_flags;
		/* 0x2A */ uint16_t render_flags;
		/* 0x2C */ uint32_t base_flags;
		/* 0x30 */ phInst* ph_inst;
		PAD(0x30 + 8, 0x48) class fwDrawData* draw_handler;
		/* 0x50 */ fwDynamicEntityComponent* m_DynamicEntityComponent;
		PAD(0x50 + 8, 0x60) soup::Matrix matrix; // fwMatrixScaledTransform m_Transform;

		[[nodiscard]] const soup::Matrix& GetMatrix() const
		{
			return matrix;
		}

#ifdef STAND_DEBUG
		[[nodiscard]] std::string toString() const;
#endif

		[[nodiscard]] fwEntity* GetAttachedTo();

		[[nodiscard]] const fwAttachmentEntityExtension* GetAttachmentExtension() const
		{
			if (m_DynamicEntityComponent)
			{
				return m_DynamicEntityComponent->m_pAttachmentEntityExtension;
			}
			return nullptr;
		}
		
#if false
		[[nodiscard]] crSkeleton* GetSkeleton() const
		{
			if (const fragInst* pFragInst = GetFragInst())
			{
				if (pFragInst->m_CacheEntry)
				{
					return pFragInst->m_CacheEntry->m_HierInst.skeleton;
				}
			}
			else
			{
				if (m_DynamicEntityComponent)
				{
					return m_DynamicEntityComponent->m_pSkeleton;
				}
			}
			return nullptr;
		}
#endif
	};
	static_assert(sizeof(fwEntity) == 0x60 + sizeof(soup::Matrix));
}

struct CEntityFlags
{
public:
	uint32_t bRenderDamaged : 1;				// use damaged LOD models for objects with applicable damage
	uint32_t bUseAltFadeDistance : 1;		// this object use the alt drawable distance fade on last lod
	uint32_t bIsProcObject : 1;				// object is a procedural object
	uint32_t bIsCompEntityProcObject : 1;	// object is a proc object created by comp entity
	uint32_t bIsFloatingProcObject : 1;		// object is a procedural object
	uint32_t bLightObject : 1;				// object with lights attached
	uint32_t bIsFrag : 1;						// this entity is a fragment model
	uint32_t bInMloRoom : 1;					// object is part of an MLO interior
	uint32_t bCreatedProcObjects : 1;			// if this entity has active procedural objects
	uint32_t bPossiblyTouchesWater : 1;		// this entity is possibly affected by water rendering code and is maybe rendered as if wet, can be moved to CPhysical
	uint32_t bWillSpawnPeds : 1;				// Allows the object from spawning scenario peds if it has 2deffects.
	uint32_t bAlreadyInAudioList : 2;			// all entities have this cleared when the list is reset, 1: already in list, 2: type flags set in list
	uint32_t bLightsIgnoreDayNightSetting : 1;
	uint32_t bLightsCanCastStaticShadows : 1;
	uint32_t bLightsCanCastDynamicShadows : 1;
	uint32_t bTimedEntityDelayedState : 1;
	uint32_t bIsOnFire : 1;					// is this entity on fire
	uint32_t bHasExploded : 1;				// Has this guy blown up before
	uint32_t bHasSpawnPoints : 1;
	uint32_t bAddtoMotionBlurMask : 1;		// used for motion blur mask
	uint32_t bIsEntityProcObject : 1;		// DEV ONLY - is a procedural object that was created from another entity (rather than material)
	uint32_t bAlwaysPreRender : 1;
	uint32_t trafficLightOverride : 2;				// traffic light override state: 0: red, 1: amber, 2: green, 3: none
	uint32_t bNeverDummy : 1;				// can be moved to CObject
	uint32_t bFoundInPvs : 1;					// used by objects and dummy objects, they get flagged when in the pvs for conversion
	uint32_t bPreRenderedThisFrame : 1;
	uint32_t bUpdatingThroughAnimQueue : 1;	// used when doing ProcessControl() calls through fwAnimDirector's pre-physics queue (PRE_PHYSICS_QUEUE) - could be moved to CDynamicEntity, probably
	uint32_t bUseOcclusionQuery : 1;
	uint32_t bUseMaxDistanceForWaterReflection : 1;
	uint32_t bCloseToCamera : 1;				// used by first person props, disables Z-read
};
static_assert(sizeof(CEntityFlags) == sizeof(uint32_t));

class CEntity : public rage::fwEntity
{
	INIT_PAD(rage::fwEntity, 0xB0) uint8_t visibility_flags;
	PAD(0xB0 + 1, 0xC0) CEntityFlags m_nFlags;
	PAD(0xC4, 0xD0);

	[[nodiscard]] bool isOnScreen() const noexcept
	{
		return visibility_flags & 0x10;
	}

	[[nodiscard]] const rage::phCollider* GetCollider() const noexcept
	{
		if (m_DynamicEntityComponent)
		{
			return m_DynamicEntityComponent->m_pCollider;
		}
		return nullptr;
	}

	[[nodiscard]] const rage::phArchetypeDamp* GetPhysArch() const noexcept;
};
static_assert(sizeof(CEntity) == 0xD0);

// Values for CDynamicEntity::m_nDEflags.nStatus
enum
{
	STATUS_PLAYER,
	STATUS_PHYSICS,
	STATUS_ABANDONED,
	STATUS_WRECKED,
	STATUS_PLAYER_DISABLED,
	STATUS_OUT_OF_CONTROL
};

struct CDynamicEntityFlags
{
	uint16_t nStatus : 3;							// control status
	uint16_t bFrozenByInterior : 1;					// flag to say if an entity has been frozen by an interior
	uint16_t bFrozen : 1;							// Flag to say if the entity update has been frozen this frame (could be due to an interior, or a physical fixed flag)
	uint16_t bCheckedForDead : 1;					// The dead/alive state of this entity has been checked in the script code this frame
	uint16_t bIsGolfBall : 1;						// the object is marked as a golf ball and will run some alternate or extra physics to deal with the small scale (Particularly for rolling)
	uint16_t bForcePrePhysicsAnimUpdate : 1;		// Set to true if the entity should always update its animation pre physics regardless of if it's on screen
	uint16_t bIsBreakableGlass : 1;
	uint16_t bIsOutOfMap : 1;
	uint16_t bOverridePhysicsBounds : 1;
	uint16_t bHasMovedSinceLastPreRender : 1;
	uint16_t bUseExtendedBoundingBox : 1;
	uint16_t bIsStraddlingPortal : 1;
	uint16_t nPopType : 4;
	uint16_t nPopTypePrev : 4;
	uint16_t bReplayWarpedThisFrame : 1;
};

class CDynamicEntity : public CEntity
{
public:
	/* 0xD0 */ rage::netObject* m_net_object;
	/* 0xD8 */ CDynamicEntityFlags m_nDEflags;
	/* 0xDC */ uint16_t m_randomSeed;
	/* 0xDE */ uint8_t m_FrameCountLastVisible;
	/* 0xDF */ uint8_t m_portalStraddlingContainerIndex;
	PAD(0xE0, 0xF0) CPortalTracker portal_tracker;
};

class CPhysical : public CDynamicEntity
{
	struct CPhysicalFlags
	{
		/*  0 */ uint32_t bIsInWater : 1;
		/*  1 */ uint32_t bWasInWater : 1;
		/*  2 */ uint32_t bDontLoadCollision : 1;
		/*  3 */ uint32_t bAllowFreezeIfNoCollision : 1;
		/*  4 */ uint32_t bNotDamagedByBullets : 1;
		/*  5 */ uint32_t bNotDamagedByFlames : 1;
		/*  6 */ uint32_t bNotDamagedByCollisions : 1;
		/*  7 */ uint32_t bNotDamagedByMelee : 1;
		/*  8 */ uint32_t bNotDamagedByAnything : 1;
		/*  9 */ uint32_t bNotDamagedByAnythingButHasReactions : 1;
		/* 10 */ uint32_t bOnlyDamagedByPlayer : 1;
		/* 11 */ uint32_t bIgnoresExplosions : 1;
		/* 12 */ uint32_t bOnlyDamagedByRelGroup : 1;
		/* 13 */ uint32_t bNotDamagedByRelGroup : 1;
		/* 14 */ uint32_t bOnlyDamagedWhenRunningScript : 1;
		/* 15 */ uint32_t bNotDamagedBySteam : 1;
		/* 16 */ uint32_t bNotDamagedBySmoke : 1;
		/* 17 */ uint32_t bExplodeInstantlyWhenChecked : 1;
		/* 18 */ uint32_t bFlyer : 1;
		/* 19 */ uint32_t bRenderScorched : 1;
		/* 20 */ uint32_t bCarriedByRope : 1;
		/* 21 */ uint32_t bMoved : 1;
		/* 22 */ uint32_t bPossiblyTouchesWaterIsUpToDate : 1;
		/* 23 */ uint32_t bModifiedBounds : 1;
		/* 24 */ uint32_t bIsNotBuoyant : 1;
	};
	static_assert(sizeof(CPhysicalFlags) == 0x04);

	INIT_PAD(CDynamicEntity, 0x188) CPhysicalFlags m_nPhysicalFlags;
	PAD(0x18C, 0x190) bool m_bDontResetDamageFlagsOnCleanupMissionState : 1;
	PAD(0x191, 0x194) uint32_t relationship_hash;
	PAD(0x198, 0x280) float m_health;
	/* 0x284 */ float max_health;

	rage::fwEntity* GetAttachParentForced() const
	{
		if (const rage::fwAttachmentEntityExtension* extension = GetAttachmentExtension())
		{
			return extension->m_pAttachParent;
		}
		return nullptr;
	}

	rage::fwEntity* GetChildAttachment() const
	{
		if (const rage::fwAttachmentEntityExtension* extension = GetAttachmentExtension())
		{
			return extension->m_pAttachChild;
		}
		return nullptr;
	}

	rage::fwEntity* GetSiblingAttachment() const
	{
		if (const rage::fwAttachmentEntityExtension* extension = GetAttachmentExtension())
		{
			return extension->m_pAttachSibling;
		}
		return nullptr;
	}
};
static_assert(sizeof(CPhysical) == 0x284 + 4);
static_assert(offsetof(CPhysical, relationship_hash) == 0x194);
#pragma pack(pop)
