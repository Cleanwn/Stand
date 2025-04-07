#pragma once

#include "natives_decl.hpp"
#include "Vector2Plus.hpp"

namespace Stand
{
	struct BlipUtil
	{
		static void remove(Blip blip);
		[[nodiscard]] static int vehicleTypeToSprite(const int vehType);
		[[nodiscard]] static hash_t spriteToModel(const int spriteId);
		[[nodiscard]] static Vector2Plus getCoords(const Blip blip);
		[[nodiscard]] static Vector2Plus closestBlipCoordOfType(const int spriteId);
	};
}
