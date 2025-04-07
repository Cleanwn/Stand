#pragma once

#include "CommandToggle.hpp"

namespace Stand
{
	class CommandVehBulletproofTyres : public CommandToggle
	{
	public:
		explicit CommandVehBulletproofTyres(CommandList* const parent)
			: CommandToggle(parent, LOC("VEHBPTYR"), CMDNAMES("bulletprooftyres"), NOLABEL, false, CMDFLAGS_TOGGLE & ~CMDFLAG_SUPPORTS_STATE_OPERATIONS)
		{
		}

		void onChange(Click& click) final
		{
			if (!click.isAuto())
			{
				ensureScriptThread(click, [this, click]
				{
					Click click_(click);
					auto veh = Util::getVehicle(click_);
					if (veh.isValid())
					{
						VEHICLE::SET_VEHICLE_TYRES_CAN_BURST(veh, !m_on);
					}
				});
			}
		}

		void onTickInGameViewport() final
		{
			auto veh = Util::getVehicle();
			Click click(CLICK_AUTO, TC_SCRIPT_NOYIELD);
			setStateBool(click, !VEHICLE::GET_VEHICLE_TYRES_CAN_BURST(veh));
		}

		void onTickInWebViewport() final
		{
			return onTickInWebViewportImplRedirect();
		}
	};
}
