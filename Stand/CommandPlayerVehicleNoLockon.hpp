#pragma once

#include "CommandPlayerToggle.hpp"

#include "AbstractEntity.hpp"
#include "gta_vehicle.hpp"
#include "natives.hpp"

namespace Stand
{
	class CommandPlayerVehicleNoLockon : public CommandPlayerToggle
	{
	public:
		explicit CommandPlayerVehicleNoLockon(CommandList* const parent)
			: CommandPlayerToggle(parent, LOC("NLCKON"), CMDNAMES_OBF("givenolockon"), NOLABEL, COMMANDPERM_FRIENDLY)
		{
		}

		void onEnable(Click& click) final
		{
			PP_PTR->toggleBatch(getPathEnFromParent(COMMAND_LIST_PLAYER), [](const std::vector<AbstractPlayer>& players)
			{
				for (const auto& player : players)
				{
					if (auto vehicle = player.getVehicle(); vehicle.isValid())
					{
						SOUP_IF_LIKELY (auto cveh = vehicle.getCVehicle())
						{
							if (cveh->m_bAllowHomingMissleLockOnSynced)
							{
								if (NETWORK::NETWORK_HAS_CONTROL_OF_ENTITY(vehicle))
								{
									VEHICLE::SET_VEHICLE_ALLOW_HOMING_MISSLE_LOCKON_SYNCED(vehicle, false, false);
								}
								else
								{
									NETWORK::NETWORK_REQUEST_CONTROL_OF_ENTITY(vehicle);
								}
							}
						}
					}
				}
			});
		}

		void onDisable(Click& click) final
		{
			for (auto& player : PP_PTR->getPlayers())
			{
				if (auto vehicle = player.getVehicle(); vehicle.isValid())
				{
					vehicle.requestControl(ATSTRINGHASH("CommandPlayerVehicleNoLockon"), [](AbstractEntity& veh)
					{
						VEHICLE::SET_VEHICLE_ALLOW_HOMING_MISSLE_LOCKON_SYNCED(veh, true, false);
					});
				}
			}
		}
	};
}
