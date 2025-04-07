#pragma once

#include "CommandPlayerVehicleAction.hpp"

#include "natives.hpp"

namespace Stand
{
	class CommandPlayerVehTurnAround : public CommandPlayerVehicleAction
	{
	public:
		explicit CommandPlayerVehTurnAround(CommandList* parent)
			: CommandPlayerVehicleAction(parent, LOC("TURNAROUND"), CMDNAMES_OBF("turnaround"), NOLABEL, PV_CONTROL | PV_ALLOWSELF, COMMANDPERM_AGGRESSIVE)
		{
		}

		void onClickImpl(AbstractPlayer p, AbstractEntity& veh, const Click& click) const final
		{
			const auto speed = ENTITY::GET_ENTITY_SPEED(veh);

			auto rot = veh.getRot();
			rot.z += 180.0f;
			veh.setRot(rot);

			VEHICLE::SET_VEHICLE_FORWARD_SPEED(veh, speed);
		}
	};
}
