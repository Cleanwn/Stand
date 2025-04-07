#pragma once

#include "CommandPlayerToggle.hpp"

#include "AbstractEntity.hpp"
#include "eCarLockState.hpp"
#include "gta_vehicle.hpp"

namespace Stand
{
	class CommandPlayerVehLockDoors : public CommandPlayerToggle
	{
	private:
		AbstractEntity last_veh = AbstractEntity::invalid();

	public:
		explicit CommandPlayerVehLockDoors(CommandList* parent)
			: CommandPlayerToggle(parent, LOC("LOCKD"), CMDNAMES_OBF("lock"), NOLABEL, COMMANDPERM_AGGRESSIVE)
		{
		}

		void onEnable(Click& click) final
		{
			registerScriptTickEventHandler([this]
			{
				for (const auto& p : PP_PTR->getPlayers()) // Third-eye gets complicated here. It doesn't quite align with looped features since it'll inevitably fail to fetch a second vehicle without interrupting the user.
				{
					if (auto veh = p.getVehicle(); veh.isValid())
					{
						if (last_veh.isValid() && last_veh != veh)
						{
							resetLast();
						}

						eCarLockState desired_state = m_on ? CARLOCK_LOCKED_PLAYER_INSIDE : CARLOCK_UNLOCKED;

						if (needsChanging(veh, desired_state))
						{
							setLockedState(veh, desired_state);
							last_veh = veh;
						}
					}
					else if (last_veh.isValid())
					{
						resetLast();
					}
				}

				return m_on;
			});
		}

		void onDisable(Click& click)
		{
			if (last_veh.isValid())
			{
				ensureScriptThread([this]
				{
					resetLast();
				});
			}
		}

	private:
		[[nodiscard]] static bool needsChanging(AbstractEntity& ent, eCarLockState desired_state)
		{
			SOUP_IF_LIKELY (auto cveh = ent.getCVehicle())
			{
				for (size_t i = 0; i < cveh->m_nNumDoors; ++i)
				{
					if (cveh->m_pDoors[i].m_eDoorLockState != desired_state)
					{
						return true;
					}
				}
			}

			return false;
		}

		static void setLockedState(AbstractEntity& ent, eCarLockState desired_state)
		{
			ent.requestControl(ATSTRINGHASH("CommandPlayerVehLockDoors"), [desired_state](AbstractEntity& ent)
			{
				VEHICLE::SET_VEHICLE_DOORS_LOCKED(ent, desired_state);

				SOUP_IF_LIKELY (auto cveh = ent.getCVehicle())
				{
					for (size_t i = 0; i < cveh->m_nNumDoors; ++i)
					{
						cveh->m_pDoors[i].m_eDoorLockState = desired_state;
					}
				}
			});
		}

		void resetLast()
		{
			setLockedState(last_veh, CARLOCK_UNLOCKED);
			last_veh.reset();
		}
	};
}
