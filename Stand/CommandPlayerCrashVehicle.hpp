#pragma once

#include "CommandPlayerCrash.hpp"

#include "AbstractEntity.hpp"
#include "natives.hpp"
#include "Script.hpp"
#include "Util.hpp"

namespace Stand
{
	class CommandPlayerCrashVehicle : public CommandPlayerCrash
	{
	public:
		explicit CommandPlayerCrashVehicle(CommandList* const parent)
			: CommandPlayerCrash(parent, LOC("PLYCRSH_V"), CMDNAMES_OBF("slaughter"), LOC("PLYKCK_H_B_P"))
		{
		}

		void expandDong(std::vector<AbstractPlayer>&& targets) const final
		{
			auto p = targets.at(0);
			if (p.isRockstarAdmin())
			{
				return;
			}
			p.openThirdEye([](const AbstractPlayer& p, AbstractEntity& ped, bool needs_cleanup)
			{
				auto veh = ped.getVehicle(false);
				if (!veh.isValid())
				{
					Util::toast(LIT(LANG_FMT("PLYNVEH", p.getName())));
				}
				else
				{
					for (int i = 0; i < 20; ++i)
					{
						TASK::TASK_VEHICLE_TEMP_ACTION(ped, veh, 15, 1488);
						Script::current()->yield();
					}
				}
				p.openThirdEyeCleanup(needs_cleanup);
			});
		}
	};
}
