#pragma once

#include <cstdint>

enum eEntityType : uint8_t
{
	ENTITY_TYPE_NOTHING,
	ENTITY_TYPE_BUILDING,				// CBuilding                             : CEntity
	ENTITY_TYPE_ANIMATED_BUILDING,		// CAnimatedBuilding    : CDynamicEntity : CEntity
	ENTITY_TYPE_VEHICLE,				// CVehicle : CPhysical : CDynamicEntity : CEntity
	ENTITY_TYPE_PED,					// CPed     : CPhysical : CDynamicEntity : CEntity
	ENTITY_TYPE_OBJECT,					// CObject  : CPhysical : CDynamicEntity : CEntity
	ENTITY_TYPE_DUMMY_OBJECT,			// CDummyObject                          : CEntity
	ENTITY_TYPE_PORTAL,					// CPortalInst   : CBuilding : CEntity
	ENTITY_TYPE_MLO,					// CInteriorInst : CBuilding : CEntity
	ENTITY_TYPE_NOTINPOOLS,				// Only used to trigger audio
	ENTITY_TYPE_PARTICLESYSTEM,			// CPtFxSortedEntity : CEntity
	ENTITY_TYPE_LIGHT,					// CLightEntity : CEntity
	ENTITY_TYPE_COMPOSITE,				// CCompEntity  : CEntity
	ENTITY_TYPE_INSTANCE_LIST,			// CEntityBatch : CEntityBatchBase : CEntity
	ENTITY_TYPE_GRASS_INSTANCE_LIST,	// CGrassBatch : CEntityBatchBase : CEntity
	ENTITY_TYPE_VEHICLEGLASSCOMPONENT,	// CVehicleGlassComponentEntity : CEntity

	ENTITY_TYPE_TOTAL
};
