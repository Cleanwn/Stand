#pragma once

namespace Stand
{
	struct VehicleColour
	{
		int r;
		int g;
		int b;
	};

	struct OptVehicleColour
	{
		bool is_applicable = false;
		VehicleColour colour;
	};
}
