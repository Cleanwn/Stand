#pragma once

#include "CommandToggle.hpp"

#include "AbstractEntity.hpp"
#include "Util.hpp"

namespace Stand
{
	class CommandMuteEmergencySirens : public CommandToggle
	{
	public:
		explicit CommandMuteEmergencySirens(CommandList* const parent)
			: CommandToggle(parent, LOC("MTESIRENS"), CMDNAMES_OBF("mutesirens", "sirensoff", "sirenoff"), LOC("GEN_H_LOC"))
		{
		}

		void onEnable(Click& click) final
		{
			registerScriptTickEventHandler([this]
			{
				if (g_player_veh.isValid() && g_player_veh.isOwnerOfVehicleAndDriver())
				{
					VEHICLE::SET_VEHICLE_HAS_MUTED_SIRENS(g_player_veh, m_on);
				}

				return m_on;
			});
		}
	};
}