#include "CommandLockOnToAllVehicles.hpp"

#include "AbstractEntity.hpp"
#include "gta_vehicle.hpp"

namespace Stand
{
	CommandLockOnToAllVehicles::CommandLockOnToAllVehicles(CommandList* const parent)
		: CommandToggle(parent, LOC("ANTINLCKON"), CMDNAMES_OBF("bypassnolockon"), LOC("ANTINLCKON_H"))
	{
	}

	void CommandLockOnToAllVehicles::onEnable(Click& click)
	{
		registerScriptTickEventHandler([this]
		{
			HANDLER_CHECK(m_on);
			
			AbstractEntity::getAllVehicles([this](AbstractEntity&& veh)
			{
				if (veh == g_player_veh)
				{
					CONSUMER_CONTINUE;
				}

				SOUP_IF_LIKELY (auto cveh = veh.getCVehicle())
				{
					if (!cveh->m_bAllowHomingMissleLockOnSynced) // The local flag (m_nVehicleFlags) for this eventually gets overridden, so sadly we cannot use that to reset this.
					{
						cveh->m_bAllowHomingMissleLockOnSynced = true;
						entities.emplace(veh);
					}
				}

				CONSUMER_CONTINUE;
			});

			HANDLER_END;
		});
	}

	void CommandLockOnToAllVehicles::onDisable(Click& click)
	{
		for (auto entity : entities)
		{
			if (auto a = AbstractEntity::get(entity); a.isValid())
			{
				SOUP_IF_LIKELY (auto b = a.getCVehicle())
				{
					b->m_bAllowHomingMissleLockOnSynced = false;
				}
			}
		}

		entities.clear();
	}
}