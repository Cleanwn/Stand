#pragma once

#include "CommandStandardGameVehicleColour.hpp"

namespace Stand
{
	class CommandWheelColour : public CommandStandardGameVehicleColour
	{
	public:
		explicit CommandWheelColour(CommandList* const parent)
			: CommandStandardGameVehicleColour(parent, LOC("WHLSCLR"), CMDNAMES("vehwheelcolour"), WHEELS)
		{
		}
	};
}
