#pragma once

#include "CommandToggle.hpp"

#include "AbstractEntity.hpp"
#include "AbstractPlayer.hpp"
#include "is_session.hpp"
#include "gta_vehicle.hpp"
#include "PlayerExcludes.hpp"
#include "Util.hpp"

namespace Stand
{
	class CommandLockDoors : public CommandToggle
	{
	public:
		static inline PlayerExcludes excludes{};
		static inline CommandToggle* exclude_missions = nullptr;
		static inline CommandToggle* only_when_vacant = nullptr;
		static inline CommandToggle* exclude_others_personal_vehicles = nullptr;

		explicit CommandLockDoors(CommandList* const parent)
			: CommandToggle(parent, LOC("LOCKD"), CMDNAMES_OBF("lockdoors"))
		{
		}

		void onEnable(Click& click) final
		{
			registerScriptTickEventHandler([this]
			{
				if (exclude_missions->m_on && !is_session_freeroam()) // Should be fine, since we can't enter a non-freeroam session w/o vehicle handle changing.
				{
					return this->m_on;
				}

				if (auto veh = Util::getVehicle(); veh.isValid())
				{
					auto driver = veh.getDriver();

					if (driver.exists() && driver != g_player_ped) // Someone else is driving?
					{
						return this->m_on; // Wait until they exit.
					}

					if (exclude_others_personal_vehicles->m_on)
					{
						const auto pv_owner = veh.getPersonalVehicleOwner();

						if (pv_owner.isValid() && pv_owner != g_player)
						{
							return this->m_on;
						}
					}

					if (NETWORK::NETWORK_HAS_CONTROL_OF_ENTITY(veh))
					{
						if (auto cveh = veh.getCVehicle())
						{
							if (!cveh->m_nVehicleFlags.bDontTryToEnterThisVehicleIfLockedForPlayer) // Not already toggled?
							{
								VEHICLE::SET_DONT_ALLOW_PLAYER_TO_ENTER_VEHICLE_IF_LOCKED_FOR_PLAYER(veh, TRUE);
							}
						}

						veh.setAlwaysSynch(this->m_on);

						for (const auto& p : AbstractPlayer::listExcept(true))
						{
							auto excluded = excludes.isExcluded(p);
							auto state = !(!this->m_on || excluded);

							if (!excluded
								&& this->m_on
								&& only_when_vacant->m_on
								)
							{
								state = !g_player_veh.isValid();
							}

							if ((bool)VEHICLE::GET_VEHICLE_DOORS_LOCKED_FOR_PLAYER(veh, p) != state)
							{
								VEHICLE::SET_VEHICLE_DOORS_LOCKED_FOR_PLAYER(veh, p, state);
							}
						}

						if (VEHICLE::GET_VEHICLE_DOORS_LOCKED_FOR_PLAYER(veh, g_player) == TRUE)
						{
							VEHICLE::SET_VEHICLE_DOORS_LOCKED_FOR_PLAYER(veh, g_player, FALSE);
						}
					}
					else
					{
						NETWORK::NETWORK_REQUEST_CONTROL_OF_ENTITY(veh);
					}
				}

				return this->m_on;
			});
		}
	};
}