#pragma once

#include "CommandAction.hpp"

#include "atStringHash.hpp"
#include "natives.hpp"
#include "VehicleMods.hpp"

namespace Stand
{
	class CommandPerfWithSpoiler : public CommandAction
	{
	public:
		explicit CommandPerfWithSpoiler(CommandList* const parent)
			: CommandAction(parent, LOC("PERFWSPL"), CMDNAMES("perfwithspoiler"))
		{
		}

		void onClick(Click& click) final
		{
			click.setNoResponse();
			click.ensureScriptThread([](Click& click)
			{
				const Vehicle veh = Util::getVehicle(click);
				if (veh != 0)
				{
					AbstractEntity::get(veh).requestControl(ATSTRINGHASH("perf_with_spoiler"), [](AbstractEntity& veh) {
						VEHICLE::SET_VEHICLE_MOD_KIT(veh, 0);
						if (veh.applyPerformanceUpgradeWithSpoiler())
						{
							Util::toast(LOC("PERF_T"));
						}
						else
						{
							Util::toast(LOC("PERF_E"));
						}
					}, [](AbstractEntity&)
					{
						Util::toast(LOC("VEHCTRL"));
					});
				}
			});
		}
	};
}
