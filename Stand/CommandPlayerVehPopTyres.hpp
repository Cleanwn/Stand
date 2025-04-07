#pragma once

#include "CommandPlayerVehicleSliderText.hpp"

namespace Stand
{
	class CommandPlayerVehPopTyres : public CommandPlayerVehicleSliderText
	{
	public:
		explicit CommandPlayerVehPopTyres(CommandList* parent)
			: CommandPlayerVehicleSliderText(parent, LOC("POP_TYRES"), CMDNAMES_OBF("poptyres", "poptires"), NOLABEL, {
					{-1, LOC("ALL")},
					{0, LIT("1")},
					{1, LIT("2")},
					{4, LIT("3")},
					{5, LIT("4")},
				}, PV_CONTROL | PV_ALLOWSELF, COMMANDPERM_AGGRESSIVE)
		{
		}

		void onClickImpl(AbstractPlayer p, AbstractEntity& veh, const Click& click) const final
		{
			VEHICLE::SET_VEHICLE_TYRES_CAN_BURST(veh, true);

			if (value == -1)
			{
				for (uint8_t i = 0; i != 8; ++i)
				{
					VEHICLE::SET_VEHICLE_TYRE_BURST(veh, i, true, 1000.0f);
				}
			}
			else
			{
				VEHICLE::SET_VEHICLE_TYRE_BURST(veh, (int)value, true, 1000.0f);
			}
		}
	};
}
