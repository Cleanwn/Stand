#pragma once

#include <cstdint>

namespace Stand
{
	using netObjectType_bitset_t = uint16_t;
}

enum NetworkObjectTypes
{
	NET_OBJ_TYPE_AUTOMOBILE,
	NET_OBJ_TYPE_BIKE,
	NET_OBJ_TYPE_BOAT,
	NET_OBJ_TYPE_DOOR,
	NET_OBJ_TYPE_HELI,
	NET_OBJ_TYPE_OBJECT,
	NET_OBJ_TYPE_PED,
	NET_OBJ_TYPE_PICKUP,
	NET_OBJ_TYPE_PICKUP_PLACEMENT,
	NET_OBJ_TYPE_PLANE,
	NET_OBJ_TYPE_SUBMARINE,
	NET_OBJ_TYPE_PLAYER,
	NET_OBJ_TYPE_TRAILER,
	NET_OBJ_TYPE_TRAIN,
	//NET_OBJ_TYPE_GLASS_PANE,
	NUM_NET_OBJ_TYPES
};

inline static const char* NetworkObjectType_names[] = {
	"AUTOMOBILE",
	"BIKE",
	"BOAT",
	"DOOR",
	"HELI",
	"OBJECT",
	"PED",
	"PICKUP",
	"PICKUP_PLACEMENT",
	"PLANE",
	"SUBMARINE",
	"PLAYER",
	"TRAILER",
	"TRAIN",
};

[[nodiscard]] inline const char* NetworkObjectType_toString(uint16_t type)
{
	if (type >= NUM_NET_OBJ_TYPES)
	{
		return "INVALID";
	}
	return NetworkObjectType_names[type];
}
