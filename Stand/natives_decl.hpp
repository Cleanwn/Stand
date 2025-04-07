#pragma once

#include <cstdint>

#include "gta_fwddecl.hpp"
#include "gta_player.hpp"
#include "typedecl.hpp"

namespace rage
{
	using scrNativeHash = uint64_t;

	// scrCmd
	using scrNativeHandler = void(*)(scrNativeCallContext&);
}

using Any = uint32_t;
using Hash = hash_t;
using Entity = int32_t;
using Player = player_t;
using Ped = Entity;
using Vehicle = Entity;
using Cam = int32_t;
using Object = Entity;
using Pickup = Object;
using Blip = int32_t;
using Camera = Entity;
using ScrHandle = Entity;
using Vector3 = rage::scrVector3;
using Vector4 = rage::scrVector4;
using FireId = int32_t;
using Interior = int32_t;

// Game events use -1 to indicate "no entity" to the scripts.
// However, spawn natives just have 0 hard-coded as the default return value.
// To deal with that, the Util wrappers return AbstractEntity::invalid() if the "handle" is 0.
//
// In principle, what number we use for this shouldn't matter since we have it on a macro.
#define INVALID_GUID -1

// Was -1 in SA
#define NULL_IN_SCRIPTING_LANGUAGE 0

// Used for script "network id" & netObject id even tho these are not the same...
#define NETWORK_INVALID_OBJECT_ID 0

#define INVALID_BLIP_ID 0
