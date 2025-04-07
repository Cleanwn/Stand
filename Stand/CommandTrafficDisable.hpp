#pragma once

#include "CommandListSelect.hpp"

#include "natives.hpp"

namespace Stand
{
	class CommandTrafficDisable : public CommandListSelect
	{
	public:
		explicit CommandTrafficDisable(CommandList* const parent)
			: CommandListSelect(parent, LOC("DOFF2"), { CMDNAME("notraffic") }, NOLABEL, {
				{0, LOC("DOFF"), CMDNAMES("off")},
				{1, LOC("ON"), CMDNAMES("on")},
				{2, LOC("DTRFC_2")}
			}, 0)
		{
		}

		Label getActivationName() const final
		{
			return LOC("DTRFC_A");
		}

		void onChange(Click& click, long long prev_value) final
		{
			onChangeProcessScriptTickEventHandler(click, prev_value, 1, []
			{
				STREAMING::SET_REDUCE_VEHICLE_MODEL_BUDGET(TRUE);
				STREAMING::SET_VEHICLE_POPULATION_BUDGET(0);
				VEHICLE::SET_DISTANT_CARS_ENABLED(FALSE);
			});
			onChangeProcessScriptTickEventHandler(click, prev_value, 2, []
			{
				STREAMING::SET_REDUCE_VEHICLE_MODEL_BUDGET(TRUE);
				STREAMING::SET_VEHICLE_POPULATION_BUDGET(0);
				VEHICLE::SET_DISTANT_CARS_ENABLED(FALSE);
				VEHICLE::SET_PARKED_VEHICLE_DENSITY_MULTIPLIER_THIS_FRAME(0.f);
				VEHICLE::SET_RANDOM_VEHICLE_DENSITY_MULTIPLIER_THIS_FRAME(0.f);
			});
			if (value == 0)
			{
				ensureScriptThread(click, []
				{
					STREAMING::SET_REDUCE_VEHICLE_MODEL_BUDGET(FALSE);
					STREAMING::SET_VEHICLE_POPULATION_BUDGET(3);
					VEHICLE::SET_DISTANT_CARS_ENABLED(TRUE);
					MISC::POPULATE_NOW();
				});
			}
		}
	};
}
