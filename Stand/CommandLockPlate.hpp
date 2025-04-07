#pragma once

#include "CommandToggle.hpp"

#include "CommandPlate.hpp"
#include "get_current_time_millis.hpp"
#include "regular_event.hpp"

namespace Stand
{
	class CommandLockPlate : public CommandToggle
	{
	private:
		CommandPlate* const plate = nullptr;
		AbstractEntity last_veh;
		time_t entered_veh_at = 0;

	public:
		explicit CommandLockPlate(CommandList* const parent, CommandPlate* const plate)
			: CommandToggle(parent, LOC("LCKPLT"), CMDNAMES_OBF("lockplate", "locklicenseplate")), plate(plate)
		{
		}

		void onEnable(Click& click) final
		{
			CommandPlate::locked = true;
			
			registerScriptTickEventHandler([this]
			{
				HANDLER_CHECK(m_on);

				if (auto veh = Util::getVehicle(); veh.isValid())
				{
					if (veh != last_veh)
					{
						last_veh = veh;
						entered_veh_at = get_current_time_millis();
					}
					if (veh.isOwner()
						&& GET_MILLIS_SINCE(entered_veh_at) > 1000
						&& plate->value != VEHICLE::GET_VEHICLE_NUMBER_PLATE_TEXT(veh)
						)
					{
						veh.setPlateText(plate->value.c_str());
					}
				}

				HANDLER_END;
			});
		}

		void onDisable(Click& click) final
		{
			CommandPlate::locked = false;
		}
	};
}