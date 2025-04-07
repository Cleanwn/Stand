#pragma once

#include "CommandPlayerToggle.hpp"

#include "AbstractEntity.hpp"
#include "eTaskCombatPedFlags.hpp"
#include "natives.hpp"

namespace Stand
{
	class CommandAggressiveNPCs : public CommandPlayerToggle
	{
	public:
		explicit CommandAggressiveNPCs(CommandList* const parent)
			: CommandPlayerToggle(parent, LOC("PLYAGRNPC"), { CMDNAME("aggressivenpcs"), CMDNAME("hostilenpcs") })
		{
		}
		
		void onEnable(Click& click) final
		{
			registerScriptTickEventHandler(click, [this]
			{
				if (auto ped = AbstractPlayer(this->getPlayer()).getPed(); ped.isValid())
				{
					AbstractEntity::getAllPeds([this, ped{ std::move(ped) }](AbstractEntity&& entity) mutable
					{
						if (entity.isAPlayer())
						{
							CONSUMER_CONTINUE;
						}

						if (!NETWORK::NETWORK_HAS_CONTROL_OF_ENTITY(entity))
						{
							NETWORK::NETWORK_REQUEST_CONTROL_OF_ENTITY(entity);
						}

						PED::SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(entity, true);
						entity.setCanMigrate(!m_on);

						if (auto vehicle = entity.getVehicle(); vehicle.isValid())
						{
							vehicle.setCanMigrate(!m_on);
							if (auto driver = vehicle.getDriver(); driver.isValid() && !driver.isAPlayer())
							{
								PED::SET_DRIVER_AGGRESSIVENESS(driver, 1.0f);
								TASK::TASK_VEHICLE_MISSION_PED_TARGET(driver, vehicle, ped, m_on ? 6 : 8, 100.0f, 0, 0.0f, 0.0f, true);
							}
						}
						else
						{
							TASK::TASK_COMBAT_PED(entity, ped, eTaskCombatPedFlags::COMBAT_PED_NONE, 16);
						}

						if (!m_on)
						{
							TASK::CLEAR_PED_TASKS(entity);
							TASK::TASK_SMART_FLEE_PED(entity, ped, 1000.0f, 9999, TRUE, FALSE);
						}

						CONSUMER_CONTINUE;
					});
				}

				return this->m_on;
			});
		}
	};
}