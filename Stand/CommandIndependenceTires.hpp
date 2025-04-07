#pragma once

#include "CommandToggle.hpp"

namespace Stand
{
	class CommandIndependenceTires : public CommandToggle
	{
	public:
		explicit CommandIndependenceTires(CommandList* const parent)
			: CommandToggle(parent, LOC("TIREINDEP"), CMDNAMES("independencetiresmoke"))
		{
		}

		void onChange(Click& click) final
		{
			onChangeToggleScriptTickEventHandler(click, []
			{
				auto veh = Util::getVehicle();
				VEHICLE::TOGGLE_VEHICLE_MOD(veh, 20, true);
				VEHICLE::SET_VEHICLE_TYRE_SMOKE_COLOR(veh, 0, 0, 0);
			});
		}
	};
}
