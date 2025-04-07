#pragma once

#include <string>

#include "typedecl.hpp"

#include "vehicle_items.hpp"

namespace Stand
{
	[[nodiscard]] extern std::string get_vehicle_display_name_no_manufacturer(hash_t model);
	[[nodiscard]] extern std::string get_vehicle_display_name_no_manufacturer(const char* modelName);
	[[nodiscard]] extern std::string get_vehicle_display_name_no_manufacturer(const VehicleItem* veh);
	[[nodiscard]] extern std::string get_vehicle_display_name(const VehicleItem* veh);
	[[nodiscard]] extern std::string get_vehicle_display_name(const CVehicleModelInfo* veh);
}
