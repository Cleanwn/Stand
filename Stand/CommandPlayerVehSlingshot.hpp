#pragma once

#include "CommandPlayerVehicleSliderText.hpp"

namespace Stand
{
	class CommandPlayerVehSlingshot : public CommandPlayerVehicleSliderText
	{
	public:
		explicit CommandPlayerVehSlingshot(CommandList* const parent)
			: CommandPlayerVehicleSliderText(parent, LOC("SLINGSHOT"), { CMDNAME("slingshot") }, NOLABEL, {
					{Direction::FORWARD, LOC("V_F")},
					{Direction::BACK, LOC("V_B")},
					{Direction::UP, LOC("B_MU")},
					{Direction::DOWN, LOC("B_MD")},
					{Direction::LEFT, LOC("B_ML")},
					{Direction::RIGHT, LOC("B_MR")},
				}, PV_CONTROL | PV_ALLOWSELF, COMMANDPERM_AGGRESSIVE)
		{
		}

		void onClickImpl(AbstractPlayer p, AbstractEntity& veh, const Click& click) const final
		{
			veh.slingshot((Direction)this->value);
		}
	};
}
