#include "CommandPlayerCopyVehicle.hpp"

#include "CustomisationData.hpp"
#include "FiberPool.hpp"
#include "natives.hpp"
#include "Util.hpp"
#include "VehicleEntityCustomisationIo.hpp"

namespace Stand
{
	CommandPlayerCopyVehicle::CommandPlayerCopyVehicle(CommandList* parent)
		: CommandOnPlayer(parent, LOC("CPVEH"), CMDNAMES_OBF("copyvehicle", "copyveh"))
	{
	}

	void CommandPlayerCopyVehicle::onClick(Click& click)
	{
		ensureYieldableScriptThread(click, [=]
		{
			PP_PTR->getPlayer().openThirdEye([click](const AbstractPlayer& p, AbstractEntity& ped, bool needs_cleanup)
			{
				auto veh = ped.getVehicle(false);
				if (veh.isValid())
				{
					VehicleEntityCustomisationIo cust_io{ veh };
					CustomisationData<VehicleCustomisationIo> cust_data{};
					cust_io.copyTo(cust_data);
					FiberPool::queueJob([click, cust_data{ std::move(cust_data) }]() mutable
					{
						cust_data.spawnClone(click);
					});
				}
				else
				{
					Util::toast(LANG_FMT("PLYNVEH", ped.getPlayer().getName()));
				}
				p.openThirdEyeCleanup(needs_cleanup);
			});
		});
	}
}
