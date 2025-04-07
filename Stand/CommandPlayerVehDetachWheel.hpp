#pragma once

#include "CommandPlayerVehicleSliderText.hpp"

namespace Stand
{
	class CommandPlayerVehDetachWheel : public CommandPlayerVehicleSliderText
	{
	private:
		enum WheelIndex
		{
			ALL = -1,
			ONE,
			TWO,
			THREE,
			FOUR
		};

	public:
		explicit CommandPlayerVehDetachWheel(CommandList* const parent)
			: CommandPlayerVehicleSliderText(parent, LOC("DETROIT"), CMDNAMES_OBF("detachwheel"), NOLABEL, {
					{WheelIndex::ALL, LOC("ALL")},
					{WheelIndex::ONE, LIT("1")},
					{WheelIndex::TWO, LIT("2")},
					{WheelIndex::THREE, LIT("3")},
					{WheelIndex::FOUR, LIT("4")},
				}, PV_CONTROL | PV_ALLOWSELF, COMMANDPERM_AGGRESSIVE)
		{
		}

		void onClickImpl(AbstractPlayer p, AbstractEntity& veh, const Click& click) const final
		{
			if (this->value == WheelIndex::ALL)
			{
				for (uint8_t i = 0; i != 8; ++i)
				{
					veh.detachWheel(i);
				}
			}
			else
			{
				veh.detachWheel((int)this->value);
			}
		}
	};
}
