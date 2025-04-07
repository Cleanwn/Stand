#pragma once

#include "CommandList.hpp"

#include "gta_vehicle.hpp"
#include "natives.hpp"

#define BOOST_MOD_NEVERENDING 1
#define BOOST_MOD_INSTANTREFILL 2
#define BOOST_MOD_NOREFILLTIME 3
#define BOOST_MOD_ALWAYSEMPTY 4

namespace Stand
{
	class CommandBoostMod : public CommandListSelect
	{
	private:
		bool was_active = false;

	public:
		explicit CommandBoostMod(CommandList* const parent)
			: CommandListSelect(parent, LOC("CHRGBHV"), CMDNAMES("boostcharge", "boostmod"), NOLABEL, {
				{0, LOC("NRML"), CMDNAMES("normal")},
				{BOOST_MOD_NEVERENDING, LOC("RKTBST_NVREND"), CMDNAMES("infinite"), LOC("RKTBST_NVREND_H")},
				{BOOST_MOD_INSTANTREFILL, LOC("RKTBST_INSTRFLL"), CMDNAMES("instantrefill"), LOC("RKTBST_INSTRFLL_H")},
				{BOOST_MOD_NOREFILLTIME, LOC("RKTBST_NORFLL"), CMDNAMES("norefilltime"), LOC("RKTBST_NORFLL_H")},
				{BOOST_MOD_ALWAYSEMPTY, LOC("RKTBST_EMPTY"), CMDNAMES("empty", "alwaysempty")},
			}, 0)
		{
		}

		void onChange(Click& click, long long prev_value) final
		{
			if(this->value == 0)
			{
				return;
			}
			const auto value = this->value;
			ensureScriptThread(click, [this, value]
			{
				if (value == BOOST_MOD_INSTANTREFILL)
				{
					auto veh = g_player_veh;
					if (veh.isValid() && veh.isOwnerOfVehicleAndDriver() && !VEHICLE::IS_ROCKET_BOOST_ACTIVE(veh))
					{
						//VEHICLE::_SET_VEHICLE_ROCKET_BOOST_PERCENTAGE(veh, 1);
						((CVehicle*)veh)->boost_charge = 1.25f;
					}
				}
				registerScriptTickEventHandler(TC_SCRIPT_NOYIELD, [this, value]()
				{
					static bool was_active = false;
					if (value == this->value)
					{
						auto veh = g_player_veh;
						if (veh.isValid() && veh.isOwnerOfVehicleAndDriver())
						{
							switch (value)
							{
							case BOOST_MOD_NEVERENDING:
								VEHICLE::SET_ROCKET_BOOST_FILL(veh, 1);
								//((CVehicle*)AbstractEntity::get(veh, TYPE_VEHICLE))->boost_charge = 1.25f;
								break;

							case BOOST_MOD_INSTANTREFILL:
								if (VEHICLE::IS_ROCKET_BOOST_ACTIVE(veh))
								{
									was_active = true;
								}
								else if (was_active)
								{
									VEHICLE::SET_ROCKET_BOOST_FILL(veh, 1);
									//((CVehicle*)AbstractEntity::get(veh, TYPE_VEHICLE))->boost_charge = 1.25f;
									was_active = false;
								}
								break;

							case BOOST_MOD_NOREFILLTIME:
								VEHICLE::SET_SCRIPT_ROCKET_BOOST_RECHARGE_TIME(veh, 0);
								//((CVehicle*)AbstractEntity::get(veh, TYPE_VEHICLE))->boost_recharge_speed = FLT_MAX;
								break;

							case BOOST_MOD_ALWAYSEMPTY:
								//VEHICLE::_SET_VEHICLE_ROCKET_BOOST_PERCENTAGE(veh, 0);
								auto* const ptr = (CVehicle*)veh;
								ptr->boost_recharge_speed = 0.0f;
								ptr->boost_charge = 0.0f;
								break;
							}
						}
						else if (was_active)
						{
							was_active = false;
						}
						return true;
					}
					else
					{
						auto veh = g_player_ped.getVehicle(true);
						if (veh.isValid() && veh.isOwnerOfVehicleAndDriver())
						{
							switch (value)
							{
							case BOOST_MOD_NEVERENDING:
								VEHICLE::SET_ROCKET_BOOST_FILL(veh, 0);
								//((CVehicle*)AbstractEntity::get(veh, TYPE_VEHICLE))->boost_charge = 0.0f;
								break;

							case BOOST_MOD_NOREFILLTIME:
							case BOOST_MOD_ALWAYSEMPTY:
								((CVehicle*)veh)->boost_recharge_speed = 0.5f;
								break;
							}
						}
						return false;
					}
				});
			});
		}
	};
}
