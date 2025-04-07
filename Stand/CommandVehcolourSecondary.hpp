#pragma once

#include "CommandVehcolour.hpp"

namespace Stand
{
	class CommandVehcolourSecondary : public CommandVehcolour
	{
	public:
		explicit CommandVehcolourSecondary(CommandList* const parent)
			: CommandVehcolour(parent, LOC("CLRSEC"), { CMDNAME("vehsecondary"), CMDNAME("vehicleseconary") })
		{
		}

		void onVehcolourTick(bool root_readlocked) final
		{
			return onVehcolourTickImpl(root_readlocked, &VEHICLE::GET_VEHICLE_CUSTOM_SECONDARY_COLOUR);
		}

		void onChange(Click& click) final
		{
			if (click.type != CLICK_BULK)
			{
				int r = this->r->value;
				int g = this->g->value;
				int b = this->b->value;
				ensureScriptThread(click, [this, r, g, b](Click& click)
				{
					auto veh = (click.type == CLICK_AUTO ? Util::getVehicle() : Util::getVehicle(click));
					if (veh.isValid())
					{
						int p, w;
						VEHICLE::GET_VEHICLE_EXTRA_COLOURS(veh, &p, &w);

						if (finish_cmd != nullptr)
						{
							auto finish = int(finish_cmd->value);
							if (finish == 5) // Chrome
							{
								veh.setSecondaryColour(120);
							}
							else
							{
								VEHICLE::SET_VEHICLE_MOD_COLOR_2(veh, finish, 0);
							}
						}

						VEHICLE::SET_VEHICLE_CUSTOM_SECONDARY_COLOUR(veh, r, g, b);
						VEHICLE::SET_VEHICLE_EXTRA_COLOURS(veh, p, w);
					}
				});
			}
		}
	};
}
