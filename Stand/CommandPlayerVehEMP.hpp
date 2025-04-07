#pragma once

#include "CommandPlayerVehicleAction.hpp"

#include "eExplosionTag.hpp"
#include "natives.hpp"

namespace Stand
{
	class CommandPlayerVehEMP : public CommandPlayerVehicleAction
	{
	public:
		explicit CommandPlayerVehEMP(CommandList* parent)
			: CommandPlayerVehicleAction(parent, LOC("EMP"), { CMDNAME("empveh") }, NOLABEL, PV_ALLOWSELF, COMMANDPERM_AGGRESSIVE)
		{
		}

		void onClickImpl(AbstractPlayer p, AbstractEntity& veh, const Click& click) const final
		{
			const auto pos = veh.getPos();
			FIRE::ADD_OWNED_EXPLOSION(g_player_ped, pos.x, pos.y, pos.z, EXP_TAG_EMPLAUNCHER_EMP, 1.0f, true, false, 0.0f);
		}
	};
}
