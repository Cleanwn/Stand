#pragma once

#include "CommandToggle.hpp"

namespace Stand
{
	class CommandVehDriftTyres : public CommandToggle
	{
	public:
		explicit CommandVehDriftTyres(CommandList* const parent)
			: CommandToggle(parent, LOC("VEHDRFTYR"), CMDNAMES("drifttyres", "driftyres"), NOLABEL, false, CMDFLAGS_TOGGLE & ~CMDFLAG_SUPPORTS_STATE_OPERATIONS)
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
						VEHICLE::SET_DRIFT_TYRES(veh, m_on);
					}
				});
			}
		}

		void onTickInGameViewport() final
		{
			auto veh = Util::getVehicle();
			Click click(CLICK_AUTO, TC_SCRIPT_NOYIELD);
			setStateBool(click, VEHICLE::GET_DRIFT_TYRES_SET(veh));
		}

		void onTickInWebViewport() final
		{
			return onTickInWebViewportImplRedirect();
		}
	};
}
