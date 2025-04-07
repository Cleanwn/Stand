#pragma once

#include "CommandToggle.hpp"

#include "AbstractModel.hpp"
#include "natives.hpp"
#include "Util.hpp"

namespace Stand
{
	class CommandTonk : public CommandToggle
	{
	public:
		explicit CommandTonk(CommandList* const parent)
			: CommandToggle(parent, LOC("TONK"), { CMDNAME("tonk"), CMDNAME("removeturret") }, LOC("GEN_H_LOC"), false, CMDFLAGS_TOGGLE | CMDFLAG_NO_SAVED_STATE)
		{
		}

		void onChange(Click& click) final
		{
			auto veh = Util::getVehicle(click);
			if (veh.isValid()
				&& veh.getModel() != ATSTRINGHASH("hunter")
				)
			{
				VEHICLE::SET_TURRET_HIDDEN(veh, 0, m_on);
				VEHICLE::SET_TURRET_HIDDEN(veh, 1, m_on);
			}
		}
	};
}
