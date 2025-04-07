#pragma once

#include "CommandToggle.hpp"

#include "VehicleMods.hpp"

namespace Stand
{
	class CommandVehmodBool : public CommandToggle
	{
	private:
		const int modType;

	public:
		explicit CommandVehmodBool(CommandList* const parent, const int modType)
			: CommandToggle(parent, VehicleMods::getName(modType), {}, NOLABEL, false, CMDFLAGS_TOGGLE & ~CMDFLAG_SUPPORTS_STATE_OPERATIONS), modType(modType)
		{
			useCommandNamesFromMenuName();
		}

		void onChange(Click& click) final
		{
			if (!click.isStand())
			{
				ensureScriptThread(click, [this, click]
				{
					Click click_(click);
					auto veh = Util::getVehicle(click_);
					if (veh.isValid())
					{
						VEHICLE::TOGGLE_VEHICLE_MOD(veh, modType, m_on);
					}
				});
			}
		}

		void onTickInGameViewport() final
		{
			auto veh = Util::getVehicle();
			Click click(CLICK_AUTO, TC_SCRIPT_NOYIELD);
			setStateBool(click, VEHICLE::IS_TOGGLE_MOD_ON(veh, modType));
		}

		void onTickInWebViewport() final
		{
			return onTickInWebViewportImplRedirect();
		}
	};
}
