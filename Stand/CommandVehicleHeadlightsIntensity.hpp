#pragma once

#include "CommandSliderFloat.hpp"

#include "natives.hpp"
#include "Util.hpp"

namespace Stand
{
	class CommandVehicleHeadlightsIntensity : public CommandSliderFloat
	{
	public:
		explicit CommandVehicleHeadlightsIntensity(CommandList* const parent)
			: CommandSliderFloat(parent, LOC("HEADLITMUL"), { CMDNAME("headlightsintensity") }, LOC("H_HILOCAL"), 0, 1000000, 100, 100)
		{
		}

		void onChange(Click& click, int prev_value) final
		{
			const int value = this->value;
			const float fvalue = (float)this->value / 100.0f;
			registerScriptTickEventHandler(click, [this, value, fvalue]()
			{
				if (value != this->value)
				{
					return false;
				}
				auto veh = Util::getVehicle();
				VEHICLE::SET_VEHICLE_LIGHT_MULTIPLIER(veh, fvalue);
				return value != 100;
			});
		}
	};
}
