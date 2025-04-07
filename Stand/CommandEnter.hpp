#pragma once

#include "CommandActionScript.hpp"

namespace Stand
{
	class CommandEnter : public CommandActionScript
	{
	private:
		GetVehicleInfo* const getVehicleInfo;

	public:
		explicit CommandEnter(CommandList* const parent, GetVehicleInfo* getVehicleInfo, std::vector<CommandName>&& command_names)
			: CommandActionScript(parent, LOC("VEH_ENT"), std::move(command_names)), getVehicleInfo(getVehicleInfo)
		{
		}

		void onClickScriptThread(Click& click) final
		{
			if (!click.canExecuteChainCommand())
			{
				return;
			}
			click.stopInputIfAllowed();
			const Vehicle veh = getVehicleInfo->func();
			if (veh == INVALID_GUID)
			{
				click.setResponse(LOC_RT(getVehicleInfo->not_found_message));
				return;
			}
			auto aveh = AbstractEntity::get(veh);
			if (aveh == g_player_veh)
			{
				click.setResponse(LOC("VEHALRDIN"));
				return;
			}
			PED::SET_PED_INTO_VEHICLE(g_player_ped, veh, aveh.getFreeSeat());
			click.setChainCommandExecuted();
		}
	};
}
