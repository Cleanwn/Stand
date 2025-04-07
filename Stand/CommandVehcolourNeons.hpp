#pragma once

#include "CommandVehcolour.hpp"
#include "Util.hpp"
#include "AbstractEntity.hpp"

namespace Stand
{
	class CommandVehcolourNeons : public CommandVehcolour
	{
	public:
		explicit CommandVehcolourNeons(CommandList* const parent)
			: CommandVehcolour(parent, LOC("CLR"), { CMDNAME("neoncolour"), CMDNAME("neonscolour") })
		{
		}
		
		void onVehcolourTick(bool root_readlocked) final
		{
			return onVehcolourTickImpl(root_readlocked, &VEHICLE::GET_VEHICLE_NEON_COLOUR);
		}

		void onChange(Click& click) final
		{
			if (click.type != CLICK_BULK)
			{
				int const r = this->r->value;
				int const g = this->g->value;
				int const b = this->b->value;

				ensureScriptThread(click, [r, g, b](Click& click)
				{
					auto veh = (click.type == CLICK_AUTO ? Util::getVehicle() : Util::getVehicle(click));
					if (veh.isValid())
					{
						VEHICLE::SET_VEHICLE_NEON_COLOUR(veh, r, g, b);
					}
				});
			}
		}
	};
}
