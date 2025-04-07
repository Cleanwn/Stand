#pragma once

#include "CommandToggle.hpp"

#include "AbstractEntity.hpp"
#include "natives.hpp"
#include "Util.hpp"

namespace Stand
{
	class CommandNoLockon : public CommandToggle
	{
	public:
		explicit CommandNoLockon(CommandList* const parent)
			: CommandToggle(parent, LOC("NLCKON"), { CMDNAME("nolockon"), CMDNAME("unlockonable") })
		{
		}

		void onEnable(Click& click) final
		{
			registerScriptTickEventHandler(click, [this]()
			{
				if (m_on)
				{
					if (g_player_veh.isValid())
					{
						VEHICLE::SET_VEHICLE_ALLOW_HOMING_MISSLE_LOCKON_SYNCED(g_player_veh, false, false);
					}
					return true;
				}
				VEHICLE::SET_VEHICLE_ALLOW_HOMING_MISSLE_LOCKON_SYNCED(Util::getVehicle(), true, false);
				return false;
			});
		}
	};
}
