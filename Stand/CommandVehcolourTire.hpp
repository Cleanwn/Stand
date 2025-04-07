#pragma once

#include "CommandVehcolour.hpp"

namespace Stand
{
	class CommandVehcolourTire : public CommandVehcolour
	{
	public:
		explicit CommandVehcolourTire(CommandList* const parent)
			: CommandVehcolour(parent, LOC("CLRTIRE"), { CMDNAME("vehtire"), CMDNAME("vehicletire") })
		{
		}

		void onVehcolourTick(bool root_readlocked) final
		{
			return onVehcolourTickImpl(root_readlocked, &VEHICLE::GET_VEHICLE_TYRE_SMOKE_COLOR, true);
		}

		void onChange(Click& click) final
		{
			if (click.type != CLICK_BULK)
			{
				int r = this->r->value;
				int g = this->g->value;
				int b = this->b->value;
				ensureScriptThread(click, [=]
				{
					Click click_(click);
					auto veh = (click.type == CLICK_AUTO ? Util::getVehicle() : Util::getVehicle(click_));
					if (veh.isValid())
					{
						VEHICLE::TOGGLE_VEHICLE_MOD(veh, 20, true);
						if (r == 0 && g == 0 && b == 0)
						{
							VEHICLE::SET_VEHICLE_TYRE_SMOKE_COLOR(veh, 1, 1, 1);
						}
						else
						{
							VEHICLE::SET_VEHICLE_TYRE_SMOKE_COLOR(veh, r, g, b);
						}
					}
				});
			}
		}
	};
}
