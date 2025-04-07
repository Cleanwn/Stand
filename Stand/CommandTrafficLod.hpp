#pragma once

#include "CommandToggle.hpp"

#include "AbstractEntity.hpp"

namespace Stand
{
	class CommandTrafficLod : public CommandToggle
	{
	public:
		explicit CommandTrafficLod(CommandList* parent)
			: CommandToggle(parent, LOC("POTAT"), CMDNAMES("trafficpotato", "trafficlod"))
		{
		}

		void onChange(Click& click) final
		{
			registerScriptTickEventHandler(click, [this]
			{
				if (m_on)
				{
					AbstractEntity::getAllVehicles([](AbstractEntity&& ent)
					{
						VEHICLE::SET_VEHICLE_LOD_MULTIPLIER(ent, 0.0f);
						CONSUMER_CONTINUE;
					});
					return true;
				}
				AbstractEntity::getAllVehicles([](AbstractEntity&& ent)
				{
					VEHICLE::SET_VEHICLE_LOD_MULTIPLIER(ent, 1.0f);
					CONSUMER_CONTINUE;
				});
				return false;
			});
		}
	};
}
