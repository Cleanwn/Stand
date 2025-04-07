#pragma once

#include "OnlineVersion.hpp"

namespace Stand
{
#pragma pack(push, 1)
	struct VehicleItem
	{
		const char* hash_name;
		const char* name;
		const char* manufacturer;
		hash_t clazz;
		OnlineVersion dlc;

		VehicleItem() = default;
		VehicleItem(const char* name, const char* manufacturer, hash_t clazz, OnlineVersion&& dlc);

		[[nodiscard]] static const VehicleItem* fromHash(const hash_t hash) noexcept;
	};
#pragma pack(pop)

	extern const VehicleItem vehicle_items[864];
}
