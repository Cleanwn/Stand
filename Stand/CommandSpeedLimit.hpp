#pragma once

#include "CommandSliderFloat.hpp"

#include "natives.hpp"
#include "FiberPool.hpp"

namespace Stand
{
	class CommandSpeedLimit : public CommandSlider
	{
	public:
		explicit CommandSpeedLimit(CommandList* const parent)
			: CommandSlider(parent, LOC("SPDLMT"), { CMDNAME("speedlimit") }, LOC("SPDLMT_H"), 0, 10000, 540, 20)
		{
		}

		void onChange(Click& click, int prev_value) final
		{
			const int value = this->value;
			if (value != 540)
			{
				registerScriptTickEventHandler(click, [=]()
				{
					if (value != this->value)
					{
						return false;
					}
					auto veh = g_player_veh;
					if (veh.isValid() && veh.isOwnerOfVehicleAndDriver())
					{
						ENTITY::SET_ENTITY_MAX_SPEED(veh, (float)value / 3.6f);
					}
					return true;
				});
			}
			else
			{
				ensureScriptThread(click, [=]
				{
					auto veh = g_player_ped.getVehicle(true);
					if (veh.isValid() && veh.isOwnerOfVehicleAndDriver())
					{
						ENTITY::SET_ENTITY_MAX_SPEED(veh, 150.0f);
					}
				});
			}
		}
	};
}
