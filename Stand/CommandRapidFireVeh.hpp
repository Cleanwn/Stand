#pragma once

#include "CommandToggle.hpp"

#include "get_current_time_millis.hpp"
#include "VehicleMods.hpp"
#include "WeaponMgr.hpp"

namespace Stand
{
	class CommandRapidFireVeh : public CommandToggle
	{
	private:
		time_t last_bomb = 0;

	public:
		explicit CommandRapidFireVeh(CommandList* const parent)
			: CommandToggle(parent, LOC("RAPFREVEH"), { CMDNAME("vehiclerapidfire"), CMDNAME("vehrapidfire") })
		{
		}

		void onChange(Click& click) final
		{
			WeaponMgr::vehicle_reload_time = m_on ? 0.085f : 1.0f;

			if (m_on)
			{
				registerScriptTickEventHandler(click, [this]
				{
					HANDLER_CHECK(m_on);

					if (Input::isCommandInputAllowed()
						&& Input::isControlPressed(INPUT_VEH_HANDBRAKE)
						&& g_player_veh.isValid()
						&& g_player_veh.isOwnerOfVehicleAndDriver()
						&& g_player_veh.getModel().isAirborne()
						&& VEHICLE::GET_ARE_BOMB_BAY_DOORS_OPEN(g_player_veh)
						&& GET_MILLIS_SINCE(last_bomb) >= 50
						)
					{
						const auto pos = g_player_veh.getBombBayPos();
						MISC::SHOOT_SINGLE_BULLET_BETWEEN_COORDS_IGNORE_ENTITY_NEW(pos.first.x, pos.first.y, pos.first.z, pos.second.x, pos.second.y, pos.second.z, 0, FALSE, getBombModel(), g_player_ped, TRUE, TRUE, -1.0f, 0, FALSE, FALSE, 0, TRUE, 1, 0, 0);
						last_bomb = get_current_time_millis();
					}

					HANDLER_END;
				});
			}
		}

	private:
		[[nodiscard]] static Hash getBombModel()
		{
			switch (VEHICLE::GET_VEHICLE_MOD(g_player_veh, VehicleMods::right_fender))
			{
			case 0:
			default:
				return ATSTRINGHASH("vehicle_weapon_bomb");

			case 1:
				return ATSTRINGHASH("vehicle_weapon_bomb_incendiary");

			case 2:
				return ATSTRINGHASH("vehicle_weapon_bomb_gas");

			case 3:
				return ATSTRINGHASH("vehicle_weapon_bomb_cluster");
			}
		}
	};
}
