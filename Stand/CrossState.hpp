#pragma once

#include "CVehicleModelInfo.hpp"

#include <soup/StructMap.hpp>

namespace Stand
{
	struct CustomVehicleModel : public CVehicleModelInfo
	{
		hash_t dlc_rpf_hash_name;
		CustomVehicleModel* next;
		char hash_name[1];
	};
	static_assert(offsetof(CustomVehicleModel, dlc_rpf_hash_name) == sizeof(CVehicleModelInfo) + 0);
	static_assert(offsetof(CustomVehicleModel, next) == sizeof(CVehicleModelInfo) + 8);
	static_assert(offsetof(CustomVehicleModel, hash_name) == sizeof(CVehicleModelInfo) + 16);

	struct CrossState
	{
		[[nodiscard]] static CrossState*& ptr();

		static constexpr uint8_t current_version_major = 0; // if breaking changes have to be made, ptr should point elsewhere
		static constexpr uint8_t current_version_minor = 1;
		uint8_t version_major;
		uint8_t version_minor;
		CrossState();

		[[nodiscard]] static CrossState& get();

		CustomVehicleModel* custom_vehicle_models_head = nullptr;
		[[nodiscard]] CustomVehicleModel*& getCustomVehicleModelsTail();

		soup::StructMap struct_map{};
	};
	static_assert(offsetof(CrossState, custom_vehicle_models_head) == 0x08);
	static_assert(offsetof(CrossState, struct_map) == 0x10);
}
