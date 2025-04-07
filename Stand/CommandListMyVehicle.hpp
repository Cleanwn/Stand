#pragma once

#include "CommandList.hpp"

#include "natives_decl.hpp"

namespace Stand
{
	using get_vehicle_t = Vehicle(*)();

	struct GetVehicleInfo
	{
		const hash_t not_found_message;
		const get_vehicle_t func;
	};


	class CommandListMyVehicle : public CommandList
	{
	private:
		GetVehicleInfo getVehicleInfo;

		static Vehicle getPersonalVehicle();
		static Vehicle getLastVehicle();

	public:
		explicit CommandListMyVehicle(CommandList* const parent, bool pvehicle);
	};
}
