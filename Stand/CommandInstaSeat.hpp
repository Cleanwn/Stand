#pragma once

#include "CommandToggle.hpp"

#include "AbstractEntity.hpp"
#include "atStringHash.hpp"
#include "gta_input.hpp"
#include "input.hpp"
#include "natives.hpp"
#include "eTaskType.hpp"

namespace Stand
{
	class CommandInstaSeat : public CommandToggle
	{
	private:
		bool gave_exit_task = false;

	public:
		explicit CommandInstaSeat(CommandList* const parent)
			: CommandToggle(parent, LOC("INSTASEAT"), { CMDNAME("instaseat"), CMDNAME("instantlyentervehicles") }, LOC("INSTASEAT_H"))
		{
		}

		void onEnable(Click& click) final
		{
			registerScriptTickEventHandler(click, [this]()
			{
				if(!m_on)
				{
					return false;
				}

				if(g_player_veh.isValid()
					&& Input::isMenuClosedOrControllerUnused()
					&& Input::isControlJustPressed(INPUT_VEH_EXIT)
					)
				{
					if(!gave_exit_task)
					{
						if(TASK::GET_IS_TASK_ACTIVE(g_player_ped, CTaskTypes::TASK_EXIT_VEHICLE))
						{
							TASK::CLEAR_PED_TASKS_IMMEDIATELY(g_player_ped);
						}

						TASK::TASK_LEAVE_VEHICLE(g_player_ped, g_player_veh, 16);
						gave_exit_task = true;
					}
				}
				else
				{
					const Vehicle veh = PED::GET_VEHICLE_PED_IS_TRYING_TO_ENTER(g_player_ped);
					if(veh != 0 && !PED::IS_PED_TRYING_TO_ENTER_A_LOCKED_VEHICLE(g_player_ped))
					{
						auto seat = VEHICLE::GET_VEHICLE_NUMBER_OF_PASSENGERS(veh, 1, 0) == 0
							? -1
							: PED::GET_SEAT_PED_IS_TRYING_TO_ENTER(g_player_ped);

						if(VEHICLE::IS_TURRET_SEAT(veh, seat))
						{
							seat = -1;
						}
						
						if(seat == -1)
						{
							auto driver_ped = AbstractEntity::get(VEHICLE::GET_PED_IN_VEHICLE_SEAT(veh, -1, false));
							if (driver_ped.isValid())
							{
								if (driver_ped.isAPlayer())
								{
									seat = 0;
								}
								else if (!TASK::GET_IS_TASK_ACTIVE(driver_ped, CTaskTypes::TASK_EXIT_VEHICLE))
								{
									TASK::TASK_LEAVE_VEHICLE(driver_ped, veh, 16);
								}
							}
						}

						PED::SET_PED_INTO_VEHICLE(g_player_ped, veh, seat);
						
						if(seat == -1)
						{
							AbstractEntity::get(veh).requestControl(ATSTRINGHASH("enter"), [](AbstractEntity& veh)
							{
								ENTITY::SET_ENTITY_ROTATION(veh, 0, 0, ENTITY::GET_ENTITY_ROTATION(veh, 2).z, 2, true);
								veh.startEngine();
							});
						}
					}
					else if(g_player_veh.isValid())
					{
						gave_exit_task = false;
					}
				}

				return true;
			});
		}
	};
}
