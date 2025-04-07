#pragma once

#include "CommandAction.hpp"

#include "AbstractEntity.hpp"
#include "natives.hpp"
#include "ScriptGlobal.hpp"
#include "Util.hpp"

namespace Stand
{
	class CommandPegasus : public CommandAction
	{
	public:
		explicit CommandPegasus(CommandList* parent)
			: CommandAction(parent, LOC("PEG"), { CMDNAME("pegasus") }, LOC("PEG_H"))
		{
		}

		void onClick(Click& click) final
		{
			if (click.inOnline())
			{
				auto veh = Util::getVehicle(click, false);
				ensureScriptThread(click, [veh{ std::move(veh) }]() mutable
				{
					DECORATOR::DECOR_SET_BOOL(veh, "CreatedByPegasus", true);
					VEHICLE::SET_VEHICLE_HAS_BEEN_OWNED_BY_PLAYER(veh, true);
					ScriptGlobal(GLOBAL_PEGASUS_VEHICLE).set<Vehicle>(veh);
					ScriptGlobal(GLOBAL_PLAYER_PEGASUS_VEHICLE).at(g_player, 1).set<Vehicle>(veh);
					ScriptGlobal(GLOBAL_RECLAIM_DISABLED).set<BOOL>(TRUE);
				});
			}
		}
	};
}
