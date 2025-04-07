#pragma once

#include "CommandSliderDynamic.hpp"

#include "AbstractEntity.hpp"
#include "natives.hpp"
#include "Util.hpp"

namespace Stand
{
	class CommandPlateType : public CommandSliderDynamic
	{
	public:
		explicit CommandPlateType(CommandList* const parent)
			: CommandSliderDynamic(parent, LOC("TYP"), CMDNAMES("platetype"), NOLABEL, 0, 0, 0)
		{
		}

		void onChange(Click& click, int prev_value) final
		{
			if (!click.isStand())
			{
				ensureScriptThread(click, [this, click]() mutable
				{
					auto veh = Util::getVehicle(click);
					if (veh.isValid())
					{
						VEHICLE::SET_VEHICLE_NUMBER_PLATE_TEXT_INDEX(veh, value);
					}
				});
			}
		}

		void onTickInGameViewport() final
		{
			auto veh = Util::getVehicle();
			if (veh.isValid())
			{
				setMaxValue(12);
				Click click(CLICK_AUTO, TC_SCRIPT_NOYIELD);
				setValue(click, VEHICLE::GET_VEHICLE_NUMBER_PLATE_TEXT_INDEX(veh));
			}
			else
			{
				setMaxValue(0);
			}
		}
	};
}
