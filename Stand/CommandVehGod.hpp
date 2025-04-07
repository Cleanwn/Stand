#pragma once

#include "CommandToggle.hpp"

#include "AbstractModel.hpp"
#include "eTaskType.hpp"
#include "gta_vehicle.hpp"
#include "natives.hpp"

namespace Stand
{
	class CommandVehGod : public CommandToggle
	{
	private:
		Vehicle last_veh = 0;

	public:
		explicit CommandVehGod(CommandList* const parent)
			: CommandToggle(parent, LOC("VEHGOD"), { CMDNAME("vehgodmode"), CMDNAME("indestructible") }, LOC("VEHGOD_H"))
		{
		}

		void onEnable(Click& click) final
		{
			ensureScriptThread(click, [this]
			{
				last_veh = 0;
				registerScriptTickEventHandler(TC_SCRIPT_NOYIELD, [this]()
				{
					auto veh = Util::getVehicle();
					if (m_on)
					{
						if (veh.isValid()
							&& veh.isOwnerOfVehicleAndDriver()
							)
						{
							if (last_veh != veh)
							{
								last_veh = veh;
								veh.repair();
							}

							// Fix for https://github.com/calamity-inc/Stand-Feedback/issues/248
							// Note that GET_VEHICLE_NUM_OF_BROKEN_OFF_PARTS always returns 4 for "police", possibly more cases like this.
							// Disabling due to issues even with openwheel vehicles: https://github.com/calamity-inc/Stand-Feedback/issues/345
#if false
							switch (veh.getModelHash())
							{
							case ATSTRINGHASH("openwheel1"):
							case ATSTRINGHASH("openwheel2"):
							case ATSTRINGHASH("formula"):
							case ATSTRINGHASH("formula2"):
								if (VEHICLE::GET_VEHICLE_NUM_OF_BROKEN_OFF_PARTS(veh) != 0)
								{
									VEHICLE::SET_VEHICLE_FIXED(veh);
									//veh.repair();
								}
								break;
							}
#endif

							veh.makeIndestructible();

							if (ENTITY::IS_ENTITY_IN_WATER(veh))
							{
								VEHICLE::SET_VEHICLE_ENGINE_ON(veh, true, true, false);
							}
						}
						return true;
					}
					if (veh.isValid())
					{
						veh.setVehicleCanBeDamaged(true);
					}
					return false;
				});
			});
		}
	};
}
