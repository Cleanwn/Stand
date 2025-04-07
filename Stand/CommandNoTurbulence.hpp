#pragma once

#include "CommandToggle.hpp"

#include "AbstractEntity.hpp"
#include "AbstractModel.hpp"
#include "gta_vehicle.hpp"
#include "Util.hpp"

namespace Stand
{
	class CommandNoTurbulence : public CommandToggle
	{
	public:
		explicit CommandNoTurbulence(CommandList* const parent)
			: CommandToggle(parent, LOC("SMTHFLY"), { CMDNAME("smoothflight"), CMDNAME("noturbulence") })
		{
		}

		void onEnable(Click& click) final
		{
			registerScriptTickEventHandler(click, [this]()
			{
				if (m_on)
				{
					setTurbulenceMultiplier(g_player_veh, 0.0f);
					return true;
				}
				auto veh = Util::getVehicle();
				setTurbulenceMultiplier(veh, 1.0f);
				return false;
			});
		}

	private:
		static void setTurbulenceMultiplier(AbstractEntity& veh, const float multiplier)
		{
			if (veh.isValid() && veh.isOwnerOfVehicleAndDriver() && VEHICLE::IS_THIS_MODEL_A_PLANE(veh.getModel()))
			{
				auto* const planeptr = (CPlane*)veh.getPointer();
				planeptr->turbulence_multiplier = multiplier;
				planeptr->wind_multiplier = multiplier;
			}
		}
	};
}
