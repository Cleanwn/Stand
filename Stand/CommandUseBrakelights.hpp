#pragma once

#include "CommandToggle.hpp"

#include "AbstractEntity.hpp"
#include "gta_vehicle.hpp"
#include "Util.hpp"

namespace Stand
{
	class CommandUseBrakelights : public CommandToggle
	{
	public:
		explicit CommandUseBrakelights(CommandList* const parent)
			: CommandToggle(parent, LOC("BRAKELIGHTS"), CMDNAMES_OBF("brakelights"))
		{
		}

		void onEnable(Click& click) final
		{
			registerScriptTickEventHandler([this]
			{
				auto veh = Util::getVehicle();

				if (veh.isValid())
				{
					if (auto cveh = veh.getCVehicle())
					{
						cveh->m_nVehicleFlags.bForceBrakeLightOn = ENTITY::GET_ENTITY_SPEED(veh) < 0.25f && m_on;
					}
				}

				return m_on;
			});
		}
	};
}