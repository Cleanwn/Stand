#pragma once

#include "CommandAction.hpp"

#include "atStringHash.hpp"
#include "natives.hpp"
#include "VehicleMods.hpp"

namespace Stand
{
	class CommandPerf : public CommandAction
	{
	public:
		explicit CommandPerf(CommandList* const parent)
			: CommandAction(parent, LOC("PERF"), { CMDNAME("performance"), CMDNAME("perf") }, LOC("PERF_H"))
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
					AbstractEntity::get(veh).requestControl(ATSTRINGHASH("perf"), [](AbstractEntity& veh)
					{
						VEHICLE::SET_VEHICLE_MOD_KIT(veh, 0);
						if (veh.applyPerformanceUpgrade())
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
