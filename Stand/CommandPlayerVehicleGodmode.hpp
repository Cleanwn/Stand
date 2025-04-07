#pragma once

#include "CommandPlayerToggle.hpp"

namespace Stand
{
	class CommandPlayerVehicleGodmode : public CommandPlayerToggle
	{
	public:
		explicit CommandPlayerVehicleGodmode(CommandList* const parent)
			: CommandPlayerToggle(parent, LOC("VEHGOD"), { CMDNAME("givevehgod") }, NOLABEL, COMMANDPERM_FRIENDLY)
		{
		}

		void onEnable(Click& click) final
		{
			registerScriptTickEventHandler(click, [this]
			{
				if (this->m_on)
				{
					for (auto& player : PP_PTR->getPlayers())
					{
						if (auto vehicle = player.getVehicle(); vehicle.isValid())
						{
							if (!vehicle.isInvulnerable())
							{
								if (!NETWORK::NETWORK_HAS_CONTROL_OF_ENTITY(vehicle))
								{
									NETWORK::NETWORK_REQUEST_CONTROL_OF_ENTITY(vehicle);
								}
								else
								{
									vehicle.repair();
									vehicle.makeIndestructible();
								}
							}
						}
					}
				}

				return this->m_on;	
			});
		}

		void onDisable(Click& click) final
		{
			for (auto& player : PP_PTR->getPlayers())
			{
				if (auto vehicle = player.getVehicle(); vehicle.isValid())
				{
					if (vehicle.isInvulnerable())
					{
						vehicle.requestControl(ATSTRINGHASH("godmode"), [](AbstractEntity& entity)
						{
							entity.setVehicleCanBeDamaged(true);
						});
					}
				}
			}
		}
	};
}