#include "gta_vehicle.hpp"

#include "CommandDlcpacks.hpp"
#include "CustomDlcMgr.hpp"
#include "Hooking.hpp"
#include "VehicleType.hpp"

bool CVehicle::InheritsFromHeli() const noexcept
{
	return (vehicle_type >= VEHICLE_TYPE_HELI && vehicle_type <= VEHICLE_TYPE_BLIMP);
}

hash_t CVehicle::getModelAsObservedByOthers() const noexcept
{
	using namespace Stand;

	hash_t model = archetype->hash;
	if (g_hooking.spoof_custom_models)
	{
		if (CustomDlcMgr::isCustomModel(model))
		{
			if (const auto spoof = CommandDlcpacks::getSpoofForVehicleType(vehicle_type); spoof.has_value())
			{
				model = spoof.value();
			}
		}
	}
	return model;
}
