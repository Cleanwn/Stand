#pragma once

#include "CommandPlayerVehicleAction.hpp"

#include "natives.hpp"
#include "TpUtil.hpp"

namespace Stand
{
	class CommandPlayerTeleportIntoVeh : public CommandPlayerVehicleAction
	{
	public:
		explicit CommandPlayerTeleportIntoVeh(CommandList* const parent)
			: CommandPlayerVehicleAction(parent, LOC("PLYTPVEH"), { CMDNAME("tpveh") })
		{
		}

		void onClickImpl(AbstractPlayer p, AbstractEntity& veh, const Click& click) const final
		{
			TpUtil::onPreTp(g_player_ped.getPos());
			PED::SET_PED_INTO_VEHICLE(g_player_ped, veh, veh.getFreeSeat());
			TpUtil::onPostTp(g_player_ped.getPos());
		}
	};
}
