#pragma once

#include "CommandToggle.hpp"

#include "pointers.hpp"

namespace Stand
{
	struct CommandWheelOnWheels : public CommandToggle
	{
		CommandWheelOnWheels(CommandList* const parent)
			: CommandToggle(parent, LOC("WHEELONWHEELS"), CMDNAMES("wheelonwheels"), LOC("WHEELONWHEELS_H"))
		{
		}

		void onEnable(Click& click) final
		{
			*pointers::weaponwheel_update_isinvehicle_true = false;
		}

		void onDisable(Click& click) final
		{
			*pointers::weaponwheel_update_isinvehicle_true = true;
		}
	};
}
