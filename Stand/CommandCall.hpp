#pragma once

#include "CommandActionScript.hpp"

#include "AbstractEntity.hpp"
#include "lang.hpp"
#include "natives.hpp"
#include "Util.hpp"

namespace Stand
{
	class CommandCall : public CommandActionScript
	{
	private:
		GetVehicleInfo* const getVehicleInfo;

	public:
		explicit CommandCall(CommandList* const parent, GetVehicleInfo* getVehicleInfo, std::vector<CommandName>&& command_names)
			: CommandActionScript(parent, LOC("CALLVEH"), std::move(command_names)), getVehicleInfo(getVehicleInfo)
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
			aveh.call();
			click.setChainCommandExecuted();
		}
	};
}
